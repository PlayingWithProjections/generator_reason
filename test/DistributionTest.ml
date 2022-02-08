open TestFramework
open Lib
open! Base

let run timestamp distribution times =
  let rec run times result =
    if times > 0 then
      run (times - 1)
        (Distribution.MonthDistribution.happens timestamp distribution :: result)
    else result
  in
  run times []
;;

describe "month distribution" (fun { test; testOnly } ->
    test "without nothing" (fun { expect } ->
        let timestamp = 0. in
        let m =
          Distribution.MDistribution.init timestamp
          |> Distribution.MDistribution.add
               ~data:(Distribution.PerMonth 0 [@explicit_arity])
          |> Distribution.MDistribution.build
        in
        let distribution =
          (Distribution.MonthDistribution.ForEver
             (Distribution.Spread m [@explicit_arity])
          [@explicit_arity]) |> Distribution.MonthDistribution.create
        in
        let result =
          Distribution.MonthDistribution.happens timestamp distribution
        in
        (expect.bool result).toBe false);
    test "without a huge amount" (fun { expect } ->
        let timestamp = 0. in
        let m =
          Distribution.MDistribution.init timestamp
          |> Distribution.MDistribution.add
               ~data:(Distribution.PerMonth 10000 [@explicit_arity])
          |> Distribution.MDistribution.build
        in
        let distribution =
          (Distribution.MonthDistribution.ForEver
             (Distribution.Spread m [@explicit_arity])
          [@explicit_arity]) |> Distribution.MonthDistribution.create
        in
        let result =
          Distribution.MonthDistribution.happens timestamp distribution
        in
        (expect.bool result).toBe true);
    test "is the frequency correct one in" (fun { expect } ->
        let timestamp = 0. in
        let distribution =
          (Distribution.MonthDistribution.Number
             (1, (Steady (OneIn 1 [@explicit_arity]) [@explicit_arity]))
          [@explicit_arity])
          |> Distribution.MonthDistribution.create
        in
        (expect.bool
           (Distribution.MonthDistribution.happens timestamp distribution))
          .toBe true);
    test "is the frequency correct one in" (fun { expect } ->
        let timestamp = 0. in
        let distribution =
          (Distribution.MonthDistribution.Number
             (1, (Steady (OneIn 1 [@explicit_arity]) [@explicit_arity]))
          [@explicit_arity])
          |> Distribution.MonthDistribution.create
        in
        let result = run timestamp distribution (24 * 60 / 10) in
        let amount = List.count ~f:(fun i -> i) result in
        (expect.bool (amount = 1)).toBe true);
    testOnly "is the frequency correct one in" (fun { expect } ->
        let foo timestamp =
          let distribution =
            (Distribution.MonthDistribution.Number
               (10, (Steady (OneIn 1 [@explicit_arity]) [@explicit_arity]))
            [@explicit_arity])
            |> Distribution.MonthDistribution.create
          in
          let result = run timestamp distribution (24 * 60 / 10) in
          List.count ~f:(fun i -> i) result
        in
        let timestamp = 0. in
        let amount = foo timestamp in
        let amount2 = foo timestamp in
        (expect.bool (amount = 10)).toBe true;
        (expect.bool (amount2 = 10)).toBe true);
    test "is the frequency correct per day" (fun { expect } ->
        let timestamp = 0. in
        let m =
          Distribution.MDistribution.init timestamp
          |> Distribution.MDistribution.add
               ~data:(Distribution.PerDay 10 [@explicit_arity])
          |> Distribution.MDistribution.build
        in
        let distribution =
          (Distribution.MonthDistribution.ForEver
             (Distribution.Spread m [@explicit_arity])
          [@explicit_arity]) |> Distribution.MonthDistribution.create
        in
        let result = run timestamp distribution (24 * 60 / 10) in
        let amount = List.count ~f:(fun i -> i) result in
        (expect.bool (amount < 20)).toBe true);
    test "is the frequency correct per month" (fun { expect } ->
        let timestamp = 0. in
        let m =
          Distribution.MDistribution.init timestamp
          |> Distribution.MDistribution.add
               ~data:(Distribution.PerMonth 10 [@explicit_arity])
          |> Distribution.MDistribution.build
        in
        let distribution =
          (Distribution.MonthDistribution.ForEver
             (Distribution.Spread m [@explicit_arity])
          [@explicit_arity]) |> Distribution.MonthDistribution.create
        in
        let result = run timestamp distribution (24 * 60 / 10 * 31) in
        let amount = List.count ~f:(fun i -> i) result in
        (expect.bool (amount < 20)).toBe true))
