open Events;

let randomState = Random.State.make([|1, 2, 3|]);

let shouldHappen = every => Random.int(every) == 0 ? true : false;

let amountPerDay = amountPerDay => 1440 / amountPerDay;

let generateId = () => Uuidm.v4_gen(randomState, ());

module Quiz = {
  type t = Uuidm.t;
};

module Player = {
  type playerType =
    | Normal
    | Bot;
  type t = {
    playerId: Uuidm.t,
    playerType,
  };
  let create = timeStamp => {
    let playerId = generateId();
    (
      {playerId, playerType: Normal},
      PlayerHasRegistered({
        playerId,
        firstName: "Fix",
        lastName: "Me",
        timeStamp,
      }),
    );
  };
  let handler = (player, state, event) => {
    switch (event) {
    | MinuteHasPassed({timeStamp}) =>
      if (amountPerDay(50) |> shouldHappen) {
        Some(
          QuizWasCreated({
            id: generateId(),
            ownerId: player.playerId,
            timeStamp,
          }),
        );
      } else {
        None;
      }
    | GameWasOpened(payload) => None
    | PlayerJoinedGame(payload) => None
    | PlayerHasRegistered(payload) => None
    };
  };
};

type outcome =
  | NothingChanged
  | Update{
      newEvents: list(event),
      newPlayers: list(Player.t),
    };

module World = {
  let handler = (state, event) => {
    switch (event) {
    | MinuteHasPassed({timeStamp}) =>
      if (amountPerDay(50) |> shouldHappen) {
        let (player, playerHasRegistered) = Player.create(timeStamp);
        Update({newEvents: [playerHasRegistered], newPlayers: [player]});
      } else {
        NothingChanged;
      }
    | GameWasOpened(payload) => NothingChanged
    | PlayerJoinedGame(payload) => NothingChanged
    | PlayerHasRegistered(payload) => NothingChanged
    };
  };
};

type state = {
  events: list(event),
  players: list(Player.t),
  quizzes: list(Quiz.t),
};

let playerHandler = (state, event) => {
  List.map(
    player =>
      switch (Player.handler(player, state, event)) {
      | None => NothingChanged
      | Some(event) => Update({newEvents: [event], newPlayers: []})
      },
    state.players,
  );
};

let worldHandler = (state, event) => [World.handler(state, event)];

let handlers = [worldHandler, playerHandler];

let handleEvent = (state, event) => {
  List.map(handler => handler(state, event), handlers) |> List.flatten;
};

let rec run = (reductions, events, state) =>
  if (reductions <= 100) {
    switch (events) {
    | [] =>
      run(reductions + 1, [MinuteHasPassed({timeStamp: reductions})], state)
    | [event, ...events] =>
      let outcomes = handleEvent(state, event);
      let newState =
        List.fold_left(
          (state, outcome) =>
            switch (outcome) {
            | NothingChanged => state
            | Update({newEvents, newPlayers}) => {
                ...state,
                events: newEvents @ state.events,
                players: newPlayers @ state.players,
              }
            },
          state,
          outcomes,
        );
      run(
        reductions,
        events,
        {...newState, events: [event, ...newState.events]},
      );
    };
  } else {
    state.events;
  };

let hello = () => {
  let events = run(0, [], {events: [], players: [], quizzes: []});
  let _ = List.map(event => show_event(event) |> Console.Pipe.log, events);
  ();
};
