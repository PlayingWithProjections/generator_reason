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
let creatingQuizButNeverPlaying: unit => t;
let boringPlayer: unit => t;
let veryGoodQuizPlayer: unit => t;
let goodQuizPlayer: unit => t;
