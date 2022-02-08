type t =
  {
  createQuizDistribution: Distribution.MonthDistribution.t ;
  joinGameDistribution: Distribution.MonthDistribution.t ;
  openGameDistribution: Distribution.MonthDistribution.t ;
  answerType: answerType }
and answerType = {
  delay: float ;
  delayRange: float ;
  correctness: float }
val alwaysWinningBot : unit -> t
val creatingQuizButNeverPlaying : unit -> t
val boringPlayer : unit -> t
val veryGoodQuizPlayer : unit -> t
val goodQuizPlayer : unit -> t