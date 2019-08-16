type t = {
  createQuizDistribution: Distribution.MonthDistribution.t,
  joinGameDistribution: Distribution.MonthDistribution.t,
  answerType,
}
and answerType = {
  delay: float,
  delayRange: float,
  correctness: float,
};

let alwaysPlayingAndAlwaysWinningBot: unit => t;
