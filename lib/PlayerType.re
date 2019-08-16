open Distribution;

type t = {
  createQuizDistribution: MonthDistribution.t,
  joinGameDistribution: MonthDistribution.t,
  answerType,
}
and answerType = {
  delay: float,
  delayRange: float,
  correctness: float,
};

let alwaysPlayingAndAlwaysWinningBot = () => {
  let createQuizDistribution =
    MonthDistribution.Never |> MonthDistribution.create;
  let joinGameDistribution =
    MonthDistribution.ForEver(Steady(PerMonth(10)))
    |> MonthDistribution.create;
  let answerType = {delay: 0., delayRange: 0.1, correctness: 1.};
  {createQuizDistribution, joinGameDistribution, answerType};
};

let creatingQuizButNeverPlaying = () => {
  let createQuizDistribution =
    MonthDistribution.ForEver(Steady(PerMonth(10)))
    |> MonthDistribution.create;
  let joinGameDistribution =
    MonthDistribution.Never |> MonthDistribution.create;
  let answerType = {delay: 0., delayRange: 0., correctness: 0.};
  {createQuizDistribution, joinGameDistribution, answerType};
};
