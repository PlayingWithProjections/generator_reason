open Base;

module Quiz = {
  type quizType =
    | Normal
    | Boring
    | Popular;
  type question = {
    question: string,
    answer: string,
  };
  type t = {
    id: Uuid.t,
    quizType,
    questions: list(question),
  };
  let create = (~id, ~questions) => {
    {id, questions, quizType: Normal};
  };
};

module Player = {
  type playerType =
    | Normal
    | Bot;
  type t = {
    id: Uuid.t,
    playerType,
  };
  let create = (~id) => {id, playerType: Normal};
  let joinGameDistribution = _player => Distribution.OneIn(10);
};

module Game = {
  type t = {
    quiz: Quiz.t,
    id: Uuid.t,
    players: list(Player.t),
  };
  let create = (~quiz, ~id) => {id, quiz, players: []};
  let join = (~game, ~player) => {
    ...game,
    players: [player, ...game.players],
  };
};

type t = {
  players: list(Player.t),
  quizzes: list(Quiz.t),
  openGames: Map.t(Uuid.t, Game.t, Uuid.comparator_witness),
};

type update =
  | AddQuiz(Quiz.t)
  | AddPlayer(Player.t)
  | OpenGame(Game.t)
  | JoinGame(Uuid.t, Player.t);

let empty = {players: [], quizzes: [], openGames: Map.empty((module Uuid))};

let update = (world, updates) => {
  List.fold_left(
    ~f=
      (world, update) =>
        switch (update) {
        | AddQuiz(quiz) => {...world, quizzes: [quiz, ...world.quizzes]}
        | AddPlayer(player) => {
            ...world,
            players: [player, ...world.players],
          }
        | OpenGame(game) => {
            ...world,
            openGames:
              Map.add_exn(world.openGames, ~key=game.Game.id, ~data=game),
          }
        | JoinGame(gameId, player) =>
          let game = Map.find_exn(world.openGames, gameId);
          let game = Game.join(~game, ~player);
          {
            ...world,
            openGames:
              Map.set(world.openGames, ~key=game.Game.id, ~data=game),
          };
        },
    ~init=world,
    updates,
  );
};

let activePlayers = world => world.players;

let pickQuiz = world => Distribution.listRandom(world.quizzes);
