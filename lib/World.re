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
    answerType,
    createQuizDistribution: Distribution.MonthDistribution.t,
    joinGameDistribution: Distribution.MonthDistribution.t,
  }
  and answerType = {
    delay: float,
    delayRange: float,
    correctness: float,
  };
  let create =
      (
        ~id,
        ~playerType,
        ~answerType,
        ~createQuizDistribution,
        ~joinGameDistribution,
      ) => {
    id,
    playerType,
    createQuizDistribution,
    joinGameDistribution,
    answerType,
  };
  let answerQuestion = player => {
    let maximumTime = 120.;
    let speed =
      Distribution.gaussianCapped(
        ~mu=player.answerType.delay *. maximumTime,
        ~sigma=player.answerType.delayRange *. maximumTime,
        ~lowerBound=0.,
        ~upperBound=maximumTime,
      );
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
      Distribution.MonthDistribution.happens(
        timestamp,
        player.createQuizDistribution,
      );
  };

  let shouldJoinGame = (timestamp, player) => {
      Distribution.MonthDistribution.happens(
        timestamp,
        player.joinGameDistribution,
      );
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
  playerDistribution:
    Distribution.PercentageDistribution.t(Player.playerType),
  createPlayerDistribution: Distribution.MonthDistribution.t,
  players: list(Player.t),
  quizzes: list(Quiz.t),
};

let init = (~playerDistribution, ~createPlayerDistribution) => {
  playerDistribution,
  createPlayerDistribution,
  players: [],
  quizzes: [],
};

let createPlayerWithType = (~world, ~playerType) => {
  let id = Uuid.generateId();
  let createQuizDistribution =
    switch (playerType) {
    | Player.NeverPlayer => Distribution.MonthDistribution.Never
    | Player.Normal =>
      Distribution.MonthDistribution.ForEver(
        Distribution.Steady(Distribution.PerMonth(10)),
      )

    | Player.BotAlwasyCorrect =>
      Distribution.MonthDistribution.Number(
        100,
        Distribution.Steady(Distribution.PerDay(10)),
      )
    } |> Distribution.MonthDistribution.create;
  let joinGameDistribution =
    switch (playerType) {
    | Player.NeverPlayer => Distribution.MonthDistribution.Never
    | Player.Normal =>
      Distribution.MonthDistribution.ForEver(
        Distribution.Steady(Distribution.PerMonth(10)),
      )

    | Player.BotAlwasyCorrect =>
      Distribution.MonthDistribution.Number(
        100,
        Distribution.Steady(Distribution.PerDay(10)),
      )
    } |> Distribution.MonthDistribution.create;
  let answerType =
    switch (playerType) {
    | Player.NeverPlayer => {
        Player.delay: 1.,
        delayRange: 0.,
        correctness: 0.,
      }
    | Player.Normal => {Player.delay: 0.8, delayRange: 0.5, correctness: 0.5}
    | Player.BotAlwasyCorrect => {
        Player.delay: 0.,
        delayRange: 0.1,
        correctness: 1.,
      }
    };
  let player =
    Player.create(
      ~id,
      ~playerType,
      ~answerType,
      ~createQuizDistribution,
      ~joinGameDistribution,
    );
  switch (player.Player.playerType) {
  | Player.NeverPlayer => (id, world)
  | Player.Normal
  | Player.BotAlwasyCorrect => (
      id,
      {...world, players: [player, ...world.players]},
    )
  };
};

let createPlayer = world => {
  let playerType =
    Distribution.PercentageDistribution.pick(world.playerDistribution);
  createPlayerWithType(~world, ~playerType);
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

let playersOpeningGame = world => world.players;

let shouldCreatePlayer = (timestamp, world) => {
    Distribution.MonthDistribution.happens(
      timestamp,
      world.createPlayerDistribution,
    );
};

// We should probably remove the players that don't play anymore to keep the performance ok
// To do this, maybe we should not allow forever playing players?
let playersThatJoinAGame = (timestamp, world) => {
  List.filter_map(world.players, ~f=player =>
    Player.shouldJoinGame(timestamp, player) ? Some(player) : None
  );
};

// We should probably remove the players that don't create a quiz anymore to keep the performance ok
let playersCreatingQuiz = (timestamp, world) => {
  List.filter_map(world.players, ~f=player =>
    Player.shouldCreateQuiz(timestamp, player) ? Some(player) : None
  );
};

let pickQuiz = world => Distribution.randomFromList(world.quizzes);
