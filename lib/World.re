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
    | NeverPlayer => `AnswerTimeout
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
};

let init = playerDistribution => {
  playerDistribution,
  players: [],
  quizzes: [],
};

let createPlayer = world => {
  let playerType = Distribution.D.pick(world.playerDistribution);
  let id = Uuid.generateId();
  let player = Player.create(~id, ~playerType);
  switch (player.Player.playerType) {
  | Player.NeverPlayer => (id, world)
  | Player.Normal
  | Player.Bot => (id, {...world, players: [player, ...world.players]})
  };
};

let createQuiz = world => {
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
              Quiz.question: "Some random question",
              answer: "Some answer",
              id: Uuid.generateId(),
            },
            ...questions,
          ],
        );
      };
    generate(0, []);
  };
  let id = Uuid.generateId();
  let questions = generateRandomAmountOfQuestions();
  let quiz = Quiz.create(~id, ~questions);
  (id, questions, {...world, quizzes: [quiz, ...world.quizzes]});
};

let activePlayers = world => world.players;

let playersThatJoinAGame = world =>
  List.filter(
    ~f=player => Player.joinGameDistribution(player) |> Distribution.happens,
    world.players,
  );

let pickQuiz = world => Distribution.listRandom(world.quizzes);
