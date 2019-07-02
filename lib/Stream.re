open! Base;

let tenMinutes = 60. *. 10.;
let fiveMinutes = 60. *. 5.;
let twoMinutes = 60. *. 2.;

let createQuiz = (ownerId, timestamp, world) => {
  let (quizId, questions, world) = World.createQuiz(world);
  let timestamp = timestamp +. Random.float(fiveMinutes);
  let createEvent =
    Events.create(
      ~timestamp,
      ~type_=Events.QuizWasCreated({quizId, ownerId, quizTitle: "Todo"}),
    );
  let (events, timestamp) =
    List.fold_left(
      ~f=
        ((events, timestamp), {World.Quiz.question, answer, id}) => {
          let timestamp = timestamp +. Random.float(tenMinutes);
          (
            [
              Events.create(
                ~timestamp,
                ~type_=
                  Events.QuestionAddedToQuiz({
                    quizId,
                    questionId: id,
                    question,
                    answer,
                  }),
              ),
              ...events,
            ],
            timestamp,
          );
        },
      ~init=([createEvent], timestamp),
      questions,
    );
  let timestamp = timestamp +. Random.float(fiveMinutes);
  let publishEvent =
    Events.create(
      ~timestamp,
      ~type_=Events.QuizWasPublished({quizId: quizId}),
    );

  (world, [publishEvent, ...events]);
};

let playGame = (player, world, timestamp) => {
  // A game starts or cancels after five minutes
  // and you have 2 minutes to answer a question
  let quiz = World.pickQuiz(world);
  switch (quiz) {
  | None => []
  | Some(quiz) =>
    let gameId = Uuid.generateId();
    let timestamp = timestamp +. Random.float(fiveMinutes);
    let events = [
      Events.create(
        ~timestamp,
        ~type_=
          Events.GameWasOpened({
            quizId: quiz.World.Quiz.id,
            gameId,
            playerId: player.World.Player.id,
          }),
      ),
    ];
    let playersJoining = World.playersThatJoinAGame(world);
    let timestampPlusFiveMinutes = timestamp +. fiveMinutes;
    let events =
      switch (playersJoining) {
      // TODO did we have a rule that at least x amount of players have to join?
      | [] => [
          Events.create(
            ~timestamp=timestampPlusFiveMinutes,
            ~type_=Events.GameWasCancelled({gameId: gameId}),
          ),
          ...events,
        ]
      | players =>
        let events =
          List.fold_left(
            ~f=
              (events, player) =>
                [
                  Events.create(
                    ~timestamp=timestamp +. Random.float(fiveMinutes),
                    ~type_=
                      Events.PlayerJoinedGame({
                        playerId: player.World.Player.id,
                        gameId,
                      }),
                  ),
                  ...events,
                ],
            ~init=events,
            players,
          );
        let events = [
          Events.create(
            ~timestamp=timestampPlusFiveMinutes,
            ~type_=Events.GameWasStarted({gameId: gameId}),
          ),
          ...events,
        ];
        let timestamp = timestampPlusFiveMinutes;
        let (events, timestamp) =
          List.fold_left(
            ~f=
              ((events, timestamp), question) => {
                let questionEvent =
                  Events.create(
                    ~timestamp,
                    ~type_=
                      Events.QuestionWasAsked({
                        gameId,
                        questionId: question.World.Quiz.id,
                      }),
                  );
                let responseEvents =
                  List.map(
                    ~f=
                      player => {
                        let answerType = World.Player.answerQuestion(player);
                        let timestamp =
                          switch (answerType) {
                          | `AnswerCorrectly(speed) => timestamp +. speed
                          | `AnswerIncorrectly(speed) => timestamp +. speed
                          | `AnswerTimeout => timestamp +. twoMinutes
                          };
                        let type_ =
                          switch (answerType) {
                          | `AnswerCorrectly(_) =>
                            Events.AnswerWasGiven({
                              questionId: question.World.Quiz.id,
                              gameId,
                              playerId: player.World.Player.id,
                              answer: question.World.Quiz.answer,
                            })
                          | `AnswerIncorrectly(_) =>
                            Events.AnswerWasGiven({
                              questionId: question.World.Quiz.id,
                              gameId,
                              playerId: player.World.Player.id,
                              answer: "TODO random incorrect answer",
                            })
                          | `AnswerTimeout =>
                            Events.TimerHasExpired({
                              questionId: question.World.Quiz.id,
                              gameId,
                              playerId: player.World.Player.id,
                            })
                          };
                        Events.create(~timestamp, ~type_);
                      },
                    players,
                  );
                let timestamp = timestamp +. twoMinutes;
                (
                  [
                    Events.create(
                      ~timestamp,
                      ~type_=
                        Events.QuestionWasCompleted({
                          gameId,
                          questionId: question.World.Quiz.id,
                        }),
                    ),
                    ...responseEvents @ [questionEvent, ...events],
                  ],
                  timestamp,
                );
              },
            ~init=(events, timestamp),
            quiz.World.Quiz.questions,
          );
        [
          Events.create(
            ~timestamp,
            ~type_=Events.GameWasFinished({gameId: gameId}),
          ),
          ...events,
        ];
      };
    events;
  };
};

let createPlayer = (timestamp, world) => {
  let timestamp = timestamp +. Random.float(tenMinutes);
  let (playerId, worldUpdate) = World.createPlayer(world);
  (
    worldUpdate,
    Events.create(
      ~timestamp,
      ~type_=
        Events.PlayerHasRegistered({
          playerId,
          firstName: Faker.firstName(),
          lastName: Faker.lastName(),
        }),
    ),
  );
};

let createPlayerHandler = (timestamp, world) => {
  let (happens, world) = World.shouldCreatePlayer(timestamp, world);
  if (happens) {
    let (world, playerHasRegistered) = createPlayer(timestamp, world);
    (world, [playerHasRegistered]);
  } else {
    (world, []);
  };
};

let createQuizHandler = (timestamp, world) => {
  let (world, playersCreatingQuiz) =
    World.playersCreatingQuiz(timestamp, world);
  List.fold_left(
    ~init=(world, []),
    ~f=
      ((world, events), player) => {
        let (world, newEvents) =
          createQuiz(player.World.Player.id, timestamp, world);
        (world, newEvents @ events);
      },
    playersCreatingQuiz,
  );
};

let playGameHandler = (timestamp, world) => {
  let playersOpeningGame = World.playersOpeningGame(world);
  List.fold_left(
    ~init=(world, []),
    ~f=
      ((world, events), player) =>
        if (Distribution.happens(Distribution.PerDay(10))) {
          let newEvents = playGame(player, world, timestamp);
          (world, newEvents @ events);
        } else {
          (world, events);
        },
    playersOpeningGame,
  );
};

let handlers = [createPlayerHandler, createQuizHandler, playGameHandler];

let handleTick = (timestamp, world) => {
  List.fold_left(
    ~init=(world, []),
    ~f=
      ((world, events), handler) => {
        let (world, newEvents) = handler(timestamp, world);
        (world, newEvents @ events);
      },
    handlers,
  );
};

let timestampRange = period => {
  open CalendarLib.Calendar;
  let now = Precise.now();
  let start = Precise.add(now, Precise.Period.opp(period));
  (Precise.to_unixfloat(start), Precise.to_unixfloat(now));
};

let rec run = (timestamp, endTimestamp, events, world) =>
  if (Float.compare(timestamp, endTimestamp) <= 0) {
    let (world, newEvents) = handleTick(timestamp, world);
    run(timestamp +. tenMinutes, endTimestamp, newEvents @ events, world);
  } else {
    events;
  };

let hello = () => {
  let period = CalendarLib.Calendar.Precise.Period.day(10);
  let (startTimestamp, endTimestamp) = timestampRange(period);
  let playerDistribution = {
    Distribution.(
      PercentageDistribution.empty()
      |> PercentageDistribution.add(
           ~i=5,
           ~outcome=World.Player.BotAlwasyCorrect,
         )
      |> PercentageDistribution.add(~i=10, ~outcome=World.Player.NeverPlayer)
      |> PercentageDistribution.rest(~outcome=World.Player.Normal)
    );
  };
  let createPlayerDistribution = {
    let m =
      Distribution.MDistribution.init(startTimestamp)
      |> Distribution.MDistribution.add(~data=Distribution.PerMonth(45))
      |> Distribution.MDistribution.add(~data=Distribution.PerMonth(45))
      |> Distribution.MDistribution.add(~data=Distribution.PerMonth(45))
      |> Distribution.MDistribution.add(~data=Distribution.PerMonth(45))
      |> Distribution.MDistribution.add(~data=Distribution.PerMonth(45))
      |> Distribution.MDistribution.build;
    (
      Distribution.MonthDistribution.ForEver,
      Distribution.MonthDistribution.Spread(m),
    );
  };
  let _events =
    run(
      startTimestamp,
      endTimestamp,
      [],
      World.init(~playerDistribution, ~createPlayerDistribution),
    );
  /* Console.log(List.length(events)); */
  /* let jsonEvents = List.rev_map(~f=Events.toJson, events); */
  /* Yojson.Basic.to_file(~len=100000, "data/0.json", `List(jsonEvents)); */
  ();
};
