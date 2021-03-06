let tenMinutes = 60. *. 10.;
let fiveMinutes = 60. *. 5.;
let twoMinutes = 60. *. 2.;

let createQuiz = (ownerId, timestamp, world) => {
  let (quizId, quizTitle, questions, world) = World.createQuiz(world);
  let timestamp = timestamp +. Random.float(fiveMinutes);
  let createEvent =
    Events.create(
      ~timestamp,
      ~type_=Events.QuizWasCreated({quizId, ownerId, quizTitle}),
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
  | None => (world, [])
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
    let playersJoining = World.playersThatJoinAGame(timestamp, world);
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
    (world, events);
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

let createPlayerHandler = (timestamp, world) =>
  if (World.shouldCreatePlayer(timestamp, world)) {
    let (world, playerHasRegistered) = createPlayer(timestamp, world);
    (world, [playerHasRegistered]);
  } else {
    (world, []);
  };

let createQuizHandler = (timestamp, world) => {
  let playersCreatingQuiz = World.playersCreatingQuiz(timestamp, world);
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
  let playersOpeningGame = World.playersOpeningGame(timestamp, world);
  List.fold_left(
    ~init=(world, []),
    ~f=
      ((world, events), player) => {
        let (world, newEvents) = playGame(player, world, timestamp);
        (world, newEvents @ events);
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

type t = {
  timeRange: TimeRange.t,
  world: World.t,
};

let create = (~timeRange, ~playerDistribution, ~createPlayerDistribution) => {
  let world = World.init(~playerDistribution, ~createPlayerDistribution);
  {timeRange, world};
};

let run = t => {
  let rec run' = (timestamp, endTimestamp, events, world) =>
    if (Float.compare(timestamp, endTimestamp) <= 0) {
      let (world, newEvents) = handleTick(timestamp, world);
      run'(timestamp +. tenMinutes, endTimestamp, newEvents @ events, world);
    } else {
      List.rev(events);
    };
  run'(
    t.timeRange.TimeRange.startTimestamp,
    t.timeRange.TimeRange.endTimestamp,
    [],
    t.world,
  );
};
