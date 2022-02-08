type t = {
  timeRange: TimeRange.t ;
  world: World.t }
val create :
  timeRange:TimeRange.t ->
    playerDistribution:PlayerType.t Distribution.PercentageDistribution.t ->
      createPlayerDistribution:Distribution.MonthDistribution.t -> t
val run : t -> Events.event list