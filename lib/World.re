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
  type t = {
    id: Uuid.t,
    answerType: PlayerType.answerType,
    createQuizDistribution: Distribution.MonthDistribution.t,
    joinGameDistribution: Distribution.MonthDistribution.t,
  };
  let create =
      (~id, ~answerType, ~createQuizDistribution, ~joinGameDistribution) => {
    id,
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
    if (Float.compare(maximumTime, speed) == 0) {
      `AnswerTimeout;
    } else {
      let correct =
        Float.compare(Random.float(player.answerType.correctness), 0.5) > 0;
      if (correct) {
        `AnswerCorrectly(speed);
      } else {
        `AnswerIncorrectly(speed);
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
  playerDistribution: Distribution.PercentageDistribution.t(PlayerType.t),
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
  let player =
    Player.create(
      ~id,
      ~answerType=playerType.PlayerType.answerType,
      ~createQuizDistribution=playerType.createQuizDistribution,
      ~joinGameDistribution=playerType.joinGameDistribution,
    );
  (id, {...world, players: [player, ...world.players]});
};

let createPlayer = world => {
  let playerType =
    Distribution.PercentageDistribution.pick(world.playerDistribution);
  createPlayerWithType(~world, ~playerType);
};

let createQuiz = world => {
  let generateRandomAmountOfQuestions = () => {
  	/* TODO: between 1 and 10 questions */
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
  let quizTitle = Faker.quizName();
  (id, quizTitle, questions, {...world, quizzes: [quiz, ...world.quizzes]});
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
