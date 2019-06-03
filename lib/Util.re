open! Base;

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
    let createEvent =
      Events.create(
        ~timestamp,
        ~type_=Events.QuizWasCreated({quizId, ownerId, quizTitle: "Todo"}),
      );
    let events =
      List.fold_left(
        ~f=
          (events, {World.Quiz.question, answer, id}) =>
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
        ~init=[createEvent],
        questions,
      );
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
  let empty = {events: [], world: World.empty};

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
  let quiz = World.pickQuiz(world);
  let gameId = Uuid.generateId();
  let events = [
    Events.create(
      ~timestamp,
      ~type_=Events.GameWasOpened({quizId: quiz.World.Quiz.id, gameId}),
    ),
  ];
  let playersJoining = World.playersThatJoinAGame(world);
  let events =
    switch (playersJoining) {
    // TODO did we have a rule that at least x amount of players have to join?
    | [] => [
        Events.create(
          ~timestamp,
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
                  ~timestamp,
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
          ~timestamp,
          ~type_=Events.GameWasStarted({gameId: gameId}),
        ),
        ...events,
      ];
      let events =
        List.fold_left(
          ~f=
            (events, question) => {
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
                      Events.create(~timestamp, ~type_);
                    },
                  players,
                );
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
              ];
            },
          ~init=events,
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
  let create = timestamp => {
    let playerId = Uuid.generateId();
    (
      World.AddPlayer(World.Player.create(~id=playerId)),
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

let worldHandler = (timestamp, _world) => {
  [
    if (Distribution.happens(Distribution.PerDay(50))) {
      let (player, playerHasRegistered) = Player.create(timestamp);
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

let tenMinutes = 60 * 10;

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
  let period = CalendarLib.Calendar.Precise.Period.day(2);
  let (startTimestamp, endTimestamp) = timestampRange(period);
  let events = run(startTimestamp, endTimestamp, [], State.empty);
  let jsonEvents = List.rev_map(~f=Events.toJson, events);
  Console.log(List.length(events));
  Yojson.Basic.to_file("data/0.json", `List(jsonEvents));
  ();
};
