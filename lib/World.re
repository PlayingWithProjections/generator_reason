open! Base;

module Quiz = {
  type quizType =
    | Normal
    | Boring
    | Popular;
  type question = {
    question: string,
    answer: string,
    id: Uuid.t,
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
  let create = (~id, ~playerType) => {id, playerType};
  let joinGameDistribution = _player => Distribution.OneIn(10);
  let answerQuestion = player =>
    switch (player.playerType) {
    | Bot => `AnswerCorrectly
    | Normal =>
      open! Poly;
      switch (Random.float(1.)) {
      | x when x >= 0.9 => `AnswerCorrectly
      | x when x >= 0.7 => `AnswerTimeout
      | _ => `AnswerIncorrectly
      };
    };
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
	playerDistribution: Distribution.D.t(Player.playerType),
  players: list(Player.t),
  quizzes: list(Quiz.t),
  openGames: Map.t(Uuid.t, Game.t, Uuid.comparator_witness),
};

type update =
  | AddQuiz(Quiz.t)
  | AddPlayer(Player.t)
  | OpenGame(Game.t)
  | JoinGame(Uuid.t, Player.t);

let init = (playerDistribution) => {playerDistribution, players: [], quizzes: [], openGames: Map.empty((module Uuid))};

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

let createPlayer = (world) => {
	let playerType = Distribution.D.pick(world.playerDistribution);
	let id = Uuid.generateId();
	let player = Player.create(~id, ~playerType);
	(id, AddPlayer(player))
}

let activePlayers = world => world.players;

let playersThatJoinAGame = world =>
  List.filter(
    ~f=player => Player.joinGameDistribution(player) |> Distribution.happens,
    world.players,
  );

let pickQuiz = world => Distribution.listRandom(world.quizzes);
