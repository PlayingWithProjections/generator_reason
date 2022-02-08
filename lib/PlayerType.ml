open Distribution

type t = {
  createQuizDistribution : MonthDistribution.t;
  joinGameDistribution : MonthDistribution.t;
  openGameDistribution : MonthDistribution.t;
  answerType : answerType;
}

and answerType = { delay : float; delayRange : float; correctness : float }

let alwaysWinningBot () =
  let createQuizDistribution =
    MonthDistribution.Never |> MonthDistribution.create
  in
  let joinGameDistribution =
    (MonthDistribution.ForEver
       (Steady (OneIn (Random.int 2) [@explicit_arity]) [@explicit_arity])
    [@explicit_arity])
    |> MonthDistribution.create
  in
  let openGameDistribution =
    MonthDistribution.Never |> MonthDistribution.create
  in
  let answerType = { delay = 0.; delayRange = 0.1; correctness = 1. } in
  {
    createQuizDistribution;
    joinGameDistribution;
    openGameDistribution;
    answerType;
  }

let creatingQuizButNeverPlaying () =
  let createQuizDistribution =
    (MonthDistribution.ForEver
       (Steady (PerMonth 10 [@explicit_arity]) [@explicit_arity])
    [@explicit_arity]) |> MonthDistribution.create
  in
  let joinGameDistribution =
    MonthDistribution.Never |> MonthDistribution.create
  in
  let openGameDistribution =
    MonthDistribution.Never |> MonthDistribution.create
  in
  let answerType = { delay = 0.; delayRange = 0.; correctness = 0. } in
  {
    createQuizDistribution;
    joinGameDistribution;
    openGameDistribution;
    answerType;
  }

let boringPlayer () =
  let createQuizDistribution =
    (MonthDistribution.Number
       (20, (Steady (PerDay 100 [@explicit_arity]) [@explicit_arity]))
    [@explicit_arity])
    |> MonthDistribution.create
  in
  let joinGameDistribution =
    (MonthDistribution.Number
       ( Random.int 40,
         (Steady (OneIn (Random.int 20) [@explicit_arity]) [@explicit_arity]) )
    [@explicit_arity])
    |> MonthDistribution.create
  in
  let openGameDistribution =
    (MonthDistribution.Number
       ( Random.int 20,
         (Steady (PerMonth (Random.int 40) [@explicit_arity]) [@explicit_arity])
       )
    [@explicit_arity])
    |> MonthDistribution.create
  in
  let answerType = { delay = 0.5; delayRange = 0.5; correctness = 0.5 } in
  {
    createQuizDistribution;
    joinGameDistribution;
    openGameDistribution;
    answerType;
  }

let veryGoodQuizPlayer () =
  let createQuizDistribution =
    MonthDistribution.Never |> MonthDistribution.create
  in
  let joinGameDistribution =
    (MonthDistribution.ForEver
       (Steady (OneIn (Random.int 10) [@explicit_arity]) [@explicit_arity])
    [@explicit_arity])
    |> MonthDistribution.create
  in
  let openGameDistribution =
    (MonthDistribution.ForEver
       (Steady (PerMonth (Random.int 40) [@explicit_arity]) [@explicit_arity])
    [@explicit_arity])
    |> MonthDistribution.create
  in
  let answerType = { delay = 0.2; delayRange = 0.5; correctness = 0.9 } in
  {
    createQuizDistribution;
    joinGameDistribution;
    openGameDistribution;
    answerType;
  }

let goodQuizPlayer () =
  let createQuizDistribution =
    MonthDistribution.Never |> MonthDistribution.create
  in
  let joinGameDistribution =
    (MonthDistribution.ForEver
       (Steady (OneIn (Random.int 10) [@explicit_arity]) [@explicit_arity])
    [@explicit_arity])
    |> MonthDistribution.create
  in
  let openGameDistribution =
    (MonthDistribution.ForEver
       (Steady (PerMonth (Random.int 40) [@explicit_arity]) [@explicit_arity])
    [@explicit_arity])
    |> MonthDistribution.create
  in
  let answerType = { delay = 0.4; delayRange = 0.6; correctness = 0.6 } in
  {
    createQuizDistribution;
    joinGameDistribution;
    openGameDistribution;
    answerType;
  }
