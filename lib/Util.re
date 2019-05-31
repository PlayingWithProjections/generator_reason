open Events;

let randomState = Random.State.make([|1, 2, 3|]);

let generateId = () => Uuid.v4_gen(randomState, ());

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
            },
            ...questions,
          ],
        );
      };
    generate(0, []);
  };
  let createQuiz = (ownerId, timeStamp) => {
    let quizId = generateId();
    let questions = generateRandomAmountOfQuestions();
    (
      World.AddQuiz(World.Quiz.create(~id=quizId, ~questions)),
      [QuizWasCreated({id: quizId, ownerId, timeStamp, quizTitle: "Todo"})]
      @ List.map(
          ({World.Quiz.question, answer}) =>
            QuestionAddedToQuiz({
              id: generateId(),
              quizId,
              question,
              answer,
              timeStamp,
            }),
          questions,
        )
      @ [QuizWasPublished({id: generateId(), quizId, timeStamp})],
    );
  };
};

module State = {
  type t = {
    events: list(event),
    world: World.t,
  };

  type outcome =
    | NothingChanged
    | Update{
        events: list(event),
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

module Player = {
  let create = timeStamp => {
    let playerId = generateId();
    (
      World.AddPlayer(World.Player.create(~id=playerId)),
      PlayerHasRegistered({
        playerId,
        firstName: "Fix",
        lastName: "Me",
        timeStamp,
      }),
    );
  };
  let handler = (player, world, event) => {
    switch (event) {
    | MinuteHasPassed({timeStamp}) =>
      let quizWasCreatedEvents =
        if (Distribution.happens(Distribution.PerDay(50))) {
          let (worldUpdate, events) =
            Quiz.createQuiz(player.World.Player.id, timeStamp);
          State.Update({events, worldUpdates: [worldUpdate]});
        } else {
          State.NothingChanged;
        };
      let gameWasOpenedEvents =
        if (Distribution.happens(Distribution.PerDay(10))) {
          let quiz = World.pickQuiz(world);
          let gameId = generateId();
          let events = [GameWasOpened({quizId: quiz.World.Quiz.id, gameId})];
          let game = World.Game.create(~quiz, ~id=gameId);
          State.Update({events, worldUpdates: [World.OpenGame(game)]});
          // Wouldn't it be easier if we played the whole game here instead of going over the ticks?
          // we could do this by going over the players, picking players that want to join, saving these as PlayerJoinedGame,
          // then if no-one joined, cancel it
          // if joined, then game was started
          // then going over all the questions and start asking them to each player
          // a player answers (correctly or incorrectly or timeout)
          // we ask the next question
          // and as such we don't need to keep complex state and look it up the whole time eiter
          // and the handlers will only care about MinuteHasPassed
        } else {
          State.NothingChanged;
        };
      [quizWasCreatedEvents, gameWasOpenedEvents];
    | GameWasOpened({gameId, _}) =>
      let distribution = World.Player.joinGameDistribution(player);

      if (Distribution.happens(distribution)) {
        [
          State.Update({
            events: [
              PlayerJoinedGame({playerId: player.World.Player.id, gameId}),
            ],
            worldUpdates: [World.JoinGame(gameId, player)],
          }),
        ];
      } else {
        [State.NothingChanged];
      };
    | PlayerJoinedGame(_)
    | PlayerHasRegistered(_)
    | QuizWasCreated(_)
    | QuestionAddedToQuiz(_)
    | QuizWasPublished(_) => [State.NothingChanged]
    };
  };
};

let worldHandler = (_world, event) => {
  [
    switch (event) {
    | MinuteHasPassed({timeStamp}) =>
      if (Distribution.happens(Distribution.PerDay(50))) {
        let (player, playerHasRegistered) = Player.create(timeStamp);
        State.Update({
          events: [playerHasRegistered],
          worldUpdates: [player],
        });
      } else {
        State.NothingChanged;
      }
    | GameWasOpened(_)
    | PlayerJoinedGame(_)
    | PlayerHasRegistered(_)
    | QuizWasCreated(_)
    | QuestionAddedToQuiz(_)
    | QuizWasPublished(_) => State.NothingChanged
    },
  ];
};

let playerHandler = (world, event) => {
  List.map(
    player => Player.handler(player, world, event),
    World.activePlayers(world),
  )
  |> List.flatten;
};

let handlers = [worldHandler, playerHandler];

let handleEvent = (world, event) => {
  List.map(handler => handler(world, event), handlers) |> List.flatten;
};

let rec run = (timeStamp, events, state) =>
  if (timeStamp <= 20000) {
    switch (events) {
    | [] =>
      run(timeStamp + 1, [MinuteHasPassed({timeStamp: timeStamp})], state)
    | [event, ...events] =>
      let outcomes = handleEvent(state.State.world, event);
      let (newState, newEvents) =
        List.fold_left(State.update, (state, []), outcomes);
      run(timeStamp, newEvents @ events, newState);
    };
  } else {
    state.State.events;
  };

let hello = () => {
  let events = run(0, [], State.empty);
  /* let _ = List.map(event => show_event(event) |> Console.Pipe.log, events); */
  Console.log(List.length(events));
  ();
};
