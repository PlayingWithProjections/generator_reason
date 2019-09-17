let basic = () => {
  let timeRange = TimeRange.daysUntilNow(~days=50);
  let playerDistribution = {
    Distribution.(
      PercentageDistribution.empty()
      |> PercentageDistribution.rest(~outcome=PlayerType.boringPlayer)
    );
  };
  let createPlayerDistribution = {
    let m =
      Distribution.MDistribution.init(timeRange.TimeRange.startTimestamp)
      |> Distribution.MDistribution.add(~data=Distribution.PerMonth(30))
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

  let jsonEvents = List.map(~f=Events.toJson, events);
  Yojson.Basic.to_file(~len=100000, "data/basic.json", `List(jsonEvents));
  ();
};

let full = () => {
  let timeRange = TimeRange.daysUntilNow(~days=500);
  let playerDistribution = {
    Distribution.(
      PercentageDistribution.empty()
      |> PercentageDistribution.add(
           ~i=1.,
           ~outcome=PlayerType.creatingQuizButNeverPlaying,
         )
      |> PercentageDistribution.add(
           ~i=0.5,
           ~outcome=PlayerType.alwaysWinningBot,
         )
      |> PercentageDistribution.add(
           ~i=1.,
           ~outcome=PlayerType.veryGoodQuizPlayer,
         )
      |> PercentageDistribution.add(
           ~i=5.,
           ~outcome=PlayerType.goodQuizPlayer,
         )
      |> PercentageDistribution.rest(~outcome=PlayerType.boringPlayer)
    );
  };
  let createPlayerDistribution = {
    let m =
      Distribution.MDistribution.init(timeRange.TimeRange.startTimestamp)
      |> Distribution.MDistribution.add(~data=Distribution.PerMonth(20))
      |> Distribution.MDistribution.add(~data=Distribution.PerMonth(40))
      |> Distribution.MDistribution.add(~data=Distribution.PerMonth(50))
      |> Distribution.MDistribution.add(~data=Distribution.PerMonth(50))
      |> Distribution.MDistribution.add(~data=Distribution.PerMonth(90))
      |> Distribution.MDistribution.add(~data=Distribution.PerMonth(120))
      |> Distribution.MDistribution.add(~data=Distribution.PerMonth(150))
      |> Distribution.MDistribution.add(~data=Distribution.PerMonth(180))
      |> Distribution.MDistribution.add(~data=Distribution.PerMonth(120))
      |> Distribution.MDistribution.add(~data=Distribution.PerMonth(110))
      |> Distribution.MDistribution.add(~data=Distribution.PerMonth(90))
      |> Distribution.MDistribution.add(~data=Distribution.PerMonth(70))
      |> Distribution.MDistribution.add(~data=Distribution.PerMonth(120))
      |> Distribution.MDistribution.add(~data=Distribution.PerMonth(125))
      |> Distribution.MDistribution.add(~data=Distribution.PerMonth(100))
      |> Distribution.MDistribution.add(~data=Distribution.PerMonth(80))
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

  let jsonEvents = List.map(~f=Events.toJson, events);
  Yojson.Basic.to_file(~len=10000000, "data/full.json", `List(jsonEvents));
  ();
};
