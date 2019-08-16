type t = {
  timeRange: TimeRange.t,
  world: World.t,
};

let create:
  (
    ~timeRange: TimeRange.t,
    ~playerDistribution: Distribution.PercentageDistribution.t(
                           World.Player.playerType,
                         ),
    ~createPlayerDistribution: Distribution.MonthDistribution.t
  ) =>
  t;

let run: t => list(Events.event);
