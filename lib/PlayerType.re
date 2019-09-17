open Distribution;

type t = {
  createQuizDistribution: MonthDistribution.t,
  joinGameDistribution: MonthDistribution.t,
  openGameDistribution: MonthDistribution.t,
  answerType,
}
and answerType = {
  delay: float,
  delayRange: float,
  correctness: float,
};

let alwaysWinningBot = () => {
	Console.log("BOT");
  let createQuizDistribution =
    MonthDistribution.Never |> MonthDistribution.create;
  let joinGameDistribution =
    MonthDistribution.ForEver(Steady(OneIn(Random.int(2))))
    |> MonthDistribution.create;
  let openGameDistribution =
    MonthDistribution.Never |> MonthDistribution.create;
  let answerType = {delay: 0., delayRange: 0.1, correctness: 1.};
  {
    createQuizDistribution,
    joinGameDistribution,
    openGameDistribution,
    answerType,
  };
};

let creatingQuizButNeverPlaying = () => {
  let createQuizDistribution =
    MonthDistribution.ForEver(Steady(PerMonth(10)))
    |> MonthDistribution.create;
  let joinGameDistribution =
    MonthDistribution.Never |> MonthDistribution.create;
  let openGameDistribution =
    MonthDistribution.Never |> MonthDistribution.create;
  let answerType = {delay: 0., delayRange: 0., correctness: 0.};
  {
    createQuizDistribution,
    joinGameDistribution,
    openGameDistribution,
    answerType,
  };
};

let boringPlayer = () => {
  let createQuizDistribution =
    MonthDistribution.Number(20, Steady(PerDay(100)))
    |> MonthDistribution.create;
  let joinGameDistribution =
    MonthDistribution.Number(Random.int(20), Steady(OneIn(Random.int(20))))
    |> MonthDistribution.create;
  let openGameDistribution =
    MonthDistribution.Number(Random.int(10), Steady(PerMonth(Random.int(40))))
    |> MonthDistribution.create;
  let answerType = {delay: 0.5, delayRange: 0.5, correctness: 0.5};
  {
    createQuizDistribution,
    joinGameDistribution,
    openGameDistribution,
    answerType,
  };
};

let veryGoodQuizPlayer = () => {
  let createQuizDistribution =
    MonthDistribution.Never |> MonthDistribution.create;
  let joinGameDistribution =
    MonthDistribution.ForEver(Steady(OneIn(Random.int(10))))
    |> MonthDistribution.create;
  let openGameDistribution =
    MonthDistribution.ForEver(Steady(PerMonth(Random.int(40))))
    |> MonthDistribution.create;
  let answerType = {delay: 0.2, delayRange: 0.5, correctness: 0.9};
  {
    createQuizDistribution,
    joinGameDistribution,
    openGameDistribution,
    answerType,
  };
};

let goodQuizPlayer = () => {
  let createQuizDistribution =
    MonthDistribution.Never |> MonthDistribution.create;
  let joinGameDistribution =
    MonthDistribution.ForEver(Steady(OneIn(Random.int(10))))
    |> MonthDistribution.create;
  let openGameDistribution =
    MonthDistribution.ForEver(Steady(PerMonth(Random.int(40))))
    |> MonthDistribution.create;
  let answerType = {delay: 0.4, delayRange: 0.6, correctness: 0.6};
  {
    createQuizDistribution,
    joinGameDistribution,
    openGameDistribution,
    answerType,
  };
};
