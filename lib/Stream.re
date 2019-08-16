open! Base;

let hello = () => {
  let timeRange = TimeRange.daysUntilNow(~days=10);
  let playerDistribution = {
    Distribution.(
      PercentageDistribution.empty()
      |> PercentageDistribution.add(
           ~i=1,
           ~outcome=PlayerType.creatingQuizButNeverPlaying(),
         )
      |> PercentageDistribution.rest(
           ~outcome=PlayerType.alwaysPlayingAndAlwaysWinningBot(),
         )
    );
  };
  let createPlayerDistribution = {
    let m =
      Distribution.MDistribution.init(timeRange.TimeRange.startTimestamp)
      |> Distribution.MDistribution.add(~data=Distribution.PerMonth(500))
      |> Distribution.MDistribution.add(~data=Distribution.PerMonth(500))
      /* |> Distribution.MDistribution.add(~data=Distribution.PerMonth(500)) */
      |> Distribution.MDistribution.build;
    Distribution.MonthDistribution.ForEver(Distribution.Spread(m))
    |> Distribution.MonthDistribution.create;
  };
  let events =
    Simulation.create(
      ~timeRange,
      ~playerDistribution,
      ~createPlayerDistribution,
    )
    |> Simulation.run;

  Console.log(List.length(events));
  let jsonEvents = List.map(~f=Events.toJson, events);
  Yojson.Basic.to_file(~len=100000, "data/0.json", `List(jsonEvents));
  ();
};
