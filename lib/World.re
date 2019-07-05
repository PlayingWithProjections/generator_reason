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
    | NeverPlayer
    | Normal
    | BotAlwasyCorrect;
  type t = {
    id: Uuid.t,
    playerType,
    createQuizDistribution: Distribution.MonthDistribution.t,
    joinGameDistribution: Distribution.MonthDistribution.t,
  };
  let create =
      (~id, ~playerType, ~createQuizDistribution, ~joinGameDistribution) => {
    id,
    playerType,
    createQuizDistribution,
    joinGameDistribution,
  };
  let answerQuestion = player => {
    let speed = Random.float(60. *. 2.);
    switch (player.playerType) {
    | BotAlwasyCorrect => `AnswerCorrectly(speed)
    | NeverPlayer => `AnswerTimeout
    | Normal =>
      open! Poly;
      switch (Random.float(1.)) {
      | x when x >= 0.9 => `AnswerCorrectly(speed)
      | x when x >= 0.7 => `AnswerTimeout
      | _ => `AnswerIncorrectly(speed)
      };
    };
  };

  let shouldCreateQuiz = (timestamp, player) => {
    let (shouldCreateQuiz, createQuizDistribution) =
      Distribution.MonthDistribution.happens(
        timestamp,
        player.createQuizDistribution,
      );
    (shouldCreateQuiz, {...player, createQuizDistribution});
  };

  let shouldJoinGame = (timestamp, player) => {
    let (shouldJoinGame, joinGameDistribution) =
      Distribution.MonthDistribution.happens(
        timestamp,
        player.joinGameDistribution,
      );
    (shouldJoinGame, {...player, joinGameDistribution});
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

type playerMap = Map.t(Uuid.t, Player.t, Uuid.comparator_witness);

type t = {
  playerDistribution:
    Distribution.PercentageDistribution.t(Player.playerType),
  createPlayerDistribution: Distribution.MonthDistribution.t,
  players: playerMap,
  quizzes: list(Quiz.t),
};

let init = (~playerDistribution, ~createPlayerDistribution) => {
  playerDistribution,
  createPlayerDistribution,
  players: Map.empty((module Uuid)),
  quizzes: [],
};

let createPlayer = world => {
  let playerType =
    Distribution.PercentageDistribution.pick(world.playerDistribution);
  let id = Uuid.generateId();
  let createQuizDistribution =
    switch (playerType) {
    | Player.NeverPlayer => Distribution.MonthDistribution.Never
    | Player.Normal =>
      Distribution.MonthDistribution.ForEver(
        Distribution.MonthDistribution.Steady(Distribution.PerMonth(10)),
      )

    | Player.BotAlwasyCorrect =>
      Distribution.MonthDistribution.Number(
        100,
        Distribution.MonthDistribution.Steady(Distribution.PerDay(10)),
      )
    };
  let joinGameDistribution =
    switch (playerType) {
    | Player.NeverPlayer => Distribution.MonthDistribution.Never
    | Player.Normal =>
      Distribution.MonthDistribution.ForEver(
        Distribution.MonthDistribution.Steady(Distribution.PerMonth(10)),
      )

    | Player.BotAlwasyCorrect =>
      Distribution.MonthDistribution.Number(
        100,
        Distribution.MonthDistribution.Steady(Distribution.PerDay(10)),
      )
    };
  let player =
    Player.create(
      ~id,
      ~playerType,
      ~createQuizDistribution,
      ~joinGameDistribution,
    );
  switch (player.Player.playerType) {
  | Player.NeverPlayer => (id, world)
  | Player.Normal
  | Player.BotAlwasyCorrect => (
      id,
      {...world, players: Map.set(world.players, ~key=id, ~data=player)},
    )
  };
};

let createQuiz = world => {
  let generateRandomAmountOfQuestions = () => {
    let nbOfQuestions = Random.int(10);
    List.map(
      ~f=
        ((question, answer)) =>
          {Quiz.question, answer, id: Uuid.generateId()},
      Faker.quiz(nbOfQuestions),
    );
  };
  let id = Uuid.generateId();
  let questions = generateRandomAmountOfQuestions();
  let quiz = Quiz.create(~id, ~questions);
  (id, questions, {...world, quizzes: [quiz, ...world.quizzes]});
};

let playersOpeningGame = world => Map.data(world.players);

// We should probably remove the players that don't create a quiz anymore to keep the performance ok
let playersCreatingQuiz = (timestamp, world) => {
  Map.fold(
    world.players,
    ~init=(world, []),
    ~f=(~key as _, ~data as player, (world, playersCreatingQuiz)) => {
      let (shouldCreate, player) =
        Player.shouldCreateQuiz(timestamp, player);
      let playersCreatingQuiz =
        shouldCreate ? [player, ...playersCreatingQuiz] : playersCreatingQuiz;
      (
        {
          ...world,
          players:
            Map.set(world.players, ~key=player.Player.id, ~data=player),
        },
        playersCreatingQuiz,
      );
    },
  );
};

let shouldCreatePlayer = (timestamp, world) => {
  let (happens, newDistribution) =
    Distribution.MonthDistribution.happens(
      timestamp,
      world.createPlayerDistribution,
    );
  (happens, {...world, createPlayerDistribution: newDistribution});
};

// We should probably remove the players that don't play anymore to keep the performance ok
// To do this, maybe we should not allow forever playing players?
let playersThatJoinAGame = (timestamp, world) =>
  Map.fold(
    world.players,
    ~init=(world, []),
    ~f=(~key as _, ~data as player, (world, playersJoiningGame)) => {
      let (shouldJoin, player) = Player.shouldJoinGame(timestamp, player);
      let playersJoiningGame =
        shouldJoin ? [player, ...playersJoiningGame] : playersJoiningGame;
      (
        {
          ...world,
          players:
            Map.set(world.players, ~key=player.Player.id, ~data=player),
        },
        playersJoiningGame,
      );
    },
  );

let pickQuiz = world => Distribution.randomFromList(world.quizzes);
