open! Base;

let tenMinutes = 60 * 10;
let fiveMinutes = 60 * 5;
let twoMinutes = 60 * 2;

module Quiz = {
  let generateRandomAmountOfQuestions = () => {
    let nbOfQuestions = Random.int(10);
    let rec generate = (i, questions) =>
      if (i > nbOfQuestions) {
        questions;
      } else {
        generate(
          i + 1,
          [
            {
              World.Quiz.question: "Some random question",
              answer: "Some answer",
              id: Uuid.generateId(),
            },
            ...questions,
          ],
        );
      };
    generate(0, []);
  };
  let createQuiz = (ownerId, timestamp) => {
    let quizId = Uuid.generateId();
    let questions = generateRandomAmountOfQuestions();
    let timestamp = timestamp + Random.int(fiveMinutes);
    let createEvent =
      Events.create(
        ~timestamp,
        ~type_=Events.QuizWasCreated({quizId, ownerId, quizTitle: "Todo"}),
      );
    let (events, timestamp) =
      List.fold_left(
        ~f=
          ((events, timestamp), {World.Quiz.question, answer, id}) => {
            let timestamp = timestamp + Random.int(tenMinutes);
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
    let timestamp = timestamp + Random.int(fiveMinutes);
    let publishEvent =
      Events.create(
        ~timestamp,
        ~type_=Events.QuizWasPublished({quizId: quizId}),
      );

    (
      World.AddQuiz(World.Quiz.create(~id=quizId, ~questions)),
      [publishEvent, ...events],
    );
  };
};

module State = {
  type t = {
    events: list(Events.event),
    world: World.t,
  };

  type outcome =
    | NothingChanged
    | Update{
        events: list(Events.event),
        worldUpdates: list(World.update),
      };
  let init(playerDistribution) = {events: [], world: World.init(playerDistribution)};

  let update = ((state, newEvents), outcome) => {
    switch (outcome) {
    | NothingChanged => (state, newEvents)
    | Update({events, worldUpdates}) => (
        {
          events: events @ state.events,
          world: World.update(state.world, worldUpdates),
        },
        events @ newEvents,
      )
    };
  };
};

let playGame = (world, timestamp) => {
  // A game starts or cancels after five minutes
  // and you have 2 minutes to answer a question
  let quiz = World.pickQuiz(world);
  let gameId = Uuid.generateId();
  let timestamp = timestamp + Random.int(fiveMinutes);
  let events = [
    Events.create(
      ~timestamp,
      ~type_=Events.GameWasOpened({quizId: quiz.World.Quiz.id, gameId}),
    ),
  ];
  let playersJoining = World.playersThatJoinAGame(world);
  let timestampPlusFiveMinutes = timestamp + fiveMinutes;
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
                  ~timestamp=timestamp + Random.int(fiveMinutes),
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
                      let type_ =
                        switch (World.Player.answerQuestion(player)) {
                        | `AnswerCorrectly =>
                          Events.AnswerWasGiven({
                            questionId: question.World.Quiz.id,
                            gameId,
                            playerId: player.World.Player.id,
                            answer: question.World.Quiz.answer,
                          })
                        | `AnswerIncorrectly =>
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
                      // TODO the speed of the answer should depend on the type of player
                      let timestamp = timestamp + Random.int(twoMinutes);
                      Events.create(~timestamp, ~type_);
                    },
                  players,
                );
              let timestamp = timestamp + twoMinutes;
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

module Player = {
  let create = (timestamp, world) => {
    let timestamp = timestamp + Random.int(tenMinutes);
    let (playerId, worldUpdate) = World.createPlayer(world);
    (
    	worldUpdate,
      Events.create(
        ~timestamp,
        ~type_=
          Events.PlayerHasRegistered({
            playerId,
            firstName: "Fix",
            lastName: "Me",
          }),
      ),
    );
  };

  let handler = (player, timestamp, world) => {
    let quizWasCreatedEvents =
      if (Distribution.happens(Distribution.PerDay(50))) {
        let (worldUpdate, events) =
          Quiz.createQuiz(player.World.Player.id, timestamp);
        State.Update({events, worldUpdates: [worldUpdate]});
      } else {
        State.NothingChanged;
      };
    let gameWasOpenedEvents =
      if (Distribution.happens(Distribution.PerDay(10))) {
        let events = playGame(world, timestamp);
        State.Update({events, worldUpdates: []});
      } else {
        State.NothingChanged;
      };
    [quizWasCreatedEvents, gameWasOpenedEvents];
  };
};

let worldHandler = (timestamp, world) => {
  [
    if (Distribution.happens(Distribution.PerDay(50))) {
      let (player, playerHasRegistered) = Player.create(timestamp, world);
      State.Update({events: [playerHasRegistered], worldUpdates: [player]});
    } else {
      State.NothingChanged;
    },
  ];
};

let playerHandler = (timestamp, world) => {
  List.map(
    ~f=player => Player.handler(player, timestamp, world),
    World.activePlayers(world),
  )
  |> List.concat;
};

let handlers = [worldHandler, playerHandler];

let handleTick = (timestamp, world) => {
  List.map(~f=handler => handler(timestamp, world), handlers) |> List.concat;
};

let timestampRange = period => {
  open CalendarLib.Calendar;
  let now = Precise.now();
  let start = Precise.add(now, Precise.Period.opp(period));
  (
    Precise.to_unixfloat(start) |> Int.of_float,
    Precise.to_unixfloat(now) |> Int.of_float,
  );
};

let rec run = (timestamp, endTimestamp, events, state) =>
  if (timestamp <= endTimestamp) {
    let outcomes = handleTick(timestamp, state.State.world);
    let (newState, newEvents) =
      List.fold_left(~f=State.update, ~init=(state, []), outcomes);
    run(timestamp + tenMinutes, endTimestamp, newEvents @ events, newState);
  } else {
    state.State.events;
  };

let hello = () => {
  let period = CalendarLib.Calendar.Precise.Period.day(5);
  let (startTimestamp, endTimestamp) = timestampRange(period);
  let playerDistribution = {
  	open Distribution;
  	D.empty()
  		|> D.add(~i=5, ~outcome=World.Player.Bot)
  		|> D.rest(~outcome=World.Player.Normal)
			};
  let events = run(startTimestamp, endTimestamp, [], State.init(playerDistribution));
  let jsonEvents = List.rev_map(~f=Events.toJson, events);
  Console.log(List.length(events));
  Yojson.Basic.to_file("data/0.json", `List(jsonEvents));
  ();
};
