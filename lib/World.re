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

module F = {
  type howMany =
    | Number(int)
    | Never
    | ForEver;
  type distribution =
    | Steady(Distribution.frequency);
  //| Distribution(list((int, perPeriod)));
  type t = (howMany, distribution);
  let happens = t => {
    let trueOrFalse = distribution => {
      switch (distribution) {
      | Steady(perPeriod) => Distribution.happens(perPeriod)
      };
    };
    switch (t) {
    | (Never, _) => (false, t)
    | (ForEver, distribution) => (trueOrFalse(distribution), t)
    | (Number(0), _) => (false, t)
    | (Number(x), distribution) =>
      if (trueOrFalse(distribution)) {
        (true, (Number(x - 1), distribution));
      } else {
        (false, (Number(x), distribution));
      }
    };
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
    createQuizDistribution: F.t,
  };
  let create = (~id, ~playerType, ~createQuizDistribution) => {
    id,
    playerType,
    createQuizDistribution,
  };
  let joinGameDistribution = _player => Distribution.OneIn(10);
  let answerQuestion = player => {
    let speed = Random.int(60 * 2);
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

  let shouldCreateQuiz = player => {
    let (shouldCreateQuiz, createQuizDistribution) =
      F.happens(player.createQuizDistribution);
    (shouldCreateQuiz, {...player, createQuizDistribution});
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
  playerDistribution: Distribution.D.t(Player.playerType),
  players: playerMap,
  quizzes: list(Quiz.t),
};

let init = playerDistribution => {
  playerDistribution,
  players: Map.empty((module Uuid)),
  quizzes: [],
};

let createPlayer = world => {
  let playerType = Distribution.D.pick(world.playerDistribution);
  let id = Uuid.generateId();
  let createQuizDistribution = (
    F.Number(1),
    F.Steady(Distribution.PerDay(10)),
  );
  let player = Player.create(~id, ~playerType, ~createQuizDistribution);
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

let playersCreatingQuiz = world => {
  Map.fold(
    world.players,
    ~init=(world, []),
    ~f=(~key as _, ~data as player, (world, playersCreatingQuiz)) => {
      let (shouldCreate, player) = Player.shouldCreateQuiz(player);
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

let playersThatJoinAGame = world =>
  List.filter(
    ~f=player => Player.joinGameDistribution(player) |> Distribution.happens,
    Map.data(world.players),
  );

let pickQuiz = world => Distribution.randomFromList(world.quizzes);
