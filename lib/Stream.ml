let basic () =
  let timeRange = TimeRange.daysUntilNow ~days:50 in
  let playerDistribution =
    let open Distribution in
    PercentageDistribution.empty ()
    |> PercentageDistribution.rest ~outcome:PlayerType.boringPlayer
  in
  let createPlayerDistribution =
    let m =
      Distribution.MDistribution.init timeRange.TimeRange.startTimestamp
      |> Distribution.MDistribution.add
           ~data:(Distribution.PerMonth 30 [@explicit_arity])
      |> Distribution.MDistribution.add
           ~data:(Distribution.PerMonth 30 [@explicit_arity])
      |> Distribution.MDistribution.add
           ~data:(Distribution.PerMonth 90 [@explicit_arity])
      |> Distribution.MDistribution.build
    in
    (Distribution.MonthDistribution.ForEver
       (Distribution.Spread m [@explicit_arity])
    [@explicit_arity]) |> Distribution.MonthDistribution.create
  in
  let events =
    Simulation.create ~timeRange ~playerDistribution ~createPlayerDistribution
    |> Simulation.run
  in
  let jsonEvents = List.map ~f:Events.toJson events in
  Yojson.Basic.to_file ~len:100000 "data/basic.json" (`List jsonEvents);
  ()

let full () =
  let timeRange = TimeRange.daysUntilNow ~days:500 in
  let playerDistribution =
    let open Distribution in
    PercentageDistribution.empty ()
    |> PercentageDistribution.add ~i:1.
         ~outcome:PlayerType.creatingQuizButNeverPlaying
    |> PercentageDistribution.add ~i:0.5 ~outcome:PlayerType.alwaysWinningBot
    |> PercentageDistribution.add ~i:1. ~outcome:PlayerType.veryGoodQuizPlayer
    |> PercentageDistribution.add ~i:5. ~outcome:PlayerType.goodQuizPlayer
    |> PercentageDistribution.rest ~outcome:PlayerType.boringPlayer
  in
  let createPlayerDistribution =
    let m =
      Distribution.MDistribution.init timeRange.TimeRange.startTimestamp
      |> Distribution.MDistribution.add
           ~data:(Distribution.PerMonth 20 [@explicit_arity])
      |> Distribution.MDistribution.add
           ~data:(Distribution.PerMonth 40 [@explicit_arity])
      |> Distribution.MDistribution.add
           ~data:(Distribution.PerMonth 50 [@explicit_arity])
      |> Distribution.MDistribution.add
           ~data:(Distribution.PerMonth 50 [@explicit_arity])
      |> Distribution.MDistribution.add
           ~data:(Distribution.PerMonth 90 [@explicit_arity])
      |> Distribution.MDistribution.add
           ~data:(Distribution.PerMonth 120 [@explicit_arity])
      |> Distribution.MDistribution.add
           ~data:(Distribution.PerMonth 150 [@explicit_arity])
      |> Distribution.MDistribution.add
           ~data:(Distribution.PerMonth 180 [@explicit_arity])
      |> Distribution.MDistribution.add
           ~data:(Distribution.PerMonth 120 [@explicit_arity])
      |> Distribution.MDistribution.add
           ~data:(Distribution.PerMonth 110 [@explicit_arity])
      |> Distribution.MDistribution.add
           ~data:(Distribution.PerMonth 90 [@explicit_arity])
      |> Distribution.MDistribution.add
           ~data:(Distribution.PerMonth 70 [@explicit_arity])
      |> Distribution.MDistribution.add
           ~data:(Distribution.PerMonth 120 [@explicit_arity])
      |> Distribution.MDistribution.add
           ~data:(Distribution.PerMonth 125 [@explicit_arity])
      |> Distribution.MDistribution.add
           ~data:(Distribution.PerMonth 100 [@explicit_arity])
      |> Distribution.MDistribution.add
           ~data:(Distribution.PerMonth 80 [@explicit_arity])
      |> Distribution.MDistribution.build
    in
    (Distribution.MonthDistribution.ForEver
       (Distribution.Spread m [@explicit_arity])
    [@explicit_arity]) |> Distribution.MonthDistribution.create
  in
  let events =
    Simulation.create ~timeRange ~playerDistribution ~createPlayerDistribution
    |> Simulation.run
  in
  let jsonEvents = List.map ~f:Events.toJson events in
  Yojson.Basic.to_file ~len:10000000 "data/full.json" (`List jsonEvents);
  ()
