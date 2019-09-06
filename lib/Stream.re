let basic = () => {
  let timeRange = TimeRange.daysUntilNow(~days=90);
  let playerDistribution = {
    Distribution.(
      PercentageDistribution.empty()
      |> PercentageDistribution.add(
           ~i=20,
           ~outcome=PlayerType.creatingQuizButNeverPlaying(),
         )
      |> PercentageDistribution.rest(~outcome=PlayerType.boringPlayer())
    );
  };
  let createPlayerDistribution = {
    let m =
      Distribution.MDistribution.init(timeRange.TimeRange.startTimestamp)
      |> Distribution.MDistribution.add(~data=Distribution.PerMonth(20))
      |> Distribution.MDistribution.add(~data=Distribution.PerMonth(30))
      |> Distribution.MDistribution.add(~data=Distribution.PerMonth(90))
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
  Yojson.Basic.to_file(~len=100000, "data/basic.json", `List(jsonEvents));
  ();
};

let full = () => {
  let timeRange = TimeRange.daysUntilNow(~days=190);
  let playerDistribution = {
    Distribution.(
      PercentageDistribution.empty()
      |> PercentageDistribution.add(
           ~i=1,
           ~outcome=PlayerType.creatingQuizButNeverPlaying(),
         )
      |> PercentageDistribution.add(
           ~i=2,
           ~outcome=PlayerType.alwaysPlayingAndAlwaysWinningBot(),
         )
      |> PercentageDistribution.add(
           ~i=1,
           ~outcome=PlayerType.veryGoodQuizPlayer(),
         )
      |> PercentageDistribution.add(
           ~i=5,
           ~outcome=PlayerType.goodQuizPlayer(),
         )
      |> PercentageDistribution.rest(~outcome=PlayerType.boringPlayer())
    );
  };
  let createPlayerDistribution = {
    let m =
      Distribution.MDistribution.init(timeRange.TimeRange.startTimestamp)
      |> Distribution.MDistribution.add(~data=Distribution.PerMonth(20))
      |> Distribution.MDistribution.add(~data=Distribution.PerMonth(40))
      |> Distribution.MDistribution.add(~data=Distribution.PerMonth(50))
      |> Distribution.MDistribution.add(~data=Distribution.PerMonth(50))
      |> Distribution.MDistribution.add(~data=Distribution.PerMonth(60))
      |> Distribution.MDistribution.add(~data=Distribution.PerMonth(90))
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
  Yojson.Basic.to_file(~len=10000000, "data/full.json", `List(jsonEvents));
  ();
};
