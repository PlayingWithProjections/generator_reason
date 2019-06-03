open! Base;
// Check the TODO later down
// Then we need to make sure timestamps work correctly
// + start using distributions for different kind of players
// mostly for a player that plays only once, or a few times or always,....
// in my current implementation there are 4,5 million events after less then 14 days...

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
    (
      World.AddQuiz(World.Quiz.create(~id=quizId, ~questions)),
      [
        Events.create(
          ~timestamp,
          ~type_=Events.QuizWasCreated({quizId, ownerId, quizTitle: "Todo"}),
        ),
      ]
      @ List.map(
          ~f=
            ({World.Quiz.question, answer, id}) =>
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
          questions,
        )
      @ [
        Events.create(
          ~timestamp,
          ~type_=Events.QuizWasPublished({quizId: quizId}),
        ),
      ],
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
  // Wouldn't it be easier if we played the whole game here instead of going over the ticks?
  // we could do this by going over the players, picking players that want to join, saving these as PlayerJoinedGame,
  // then if no-one joined, cancel it
  // if joined, then game was started
  // then going over all the questions and start asking them to each player
  // a player answers (correctly or incorrectly or timeout)
  // we ask the next question
  // and as such we don't need to keep complex state and look it up the whole time eiter
  // and the handlers will only care about MinuteHasPassed
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

let rec run = (timestamp, events, state) =>
  if (timestamp <= 1000) {
    let outcomes = handleTick(timestamp, state.State.world);
    let (newState, newEvents) =
      List.fold_left(~f=State.update, ~init=(state, []), outcomes);
    run(timestamp + 1, newEvents @ events, newState);
  } else {
    state.State.events;
  };

let hello = () => {
  let events = run(0, [], State.empty);
  let jsonEvents = List.rev_map(~f=Events.toJson, events);
  Console.log(List.length(events));
  Yojson.Basic.to_file("data/0.json", `List(jsonEvents));
  ();
};
