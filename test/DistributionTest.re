open TestFramework;
open Lib;
open! Base;

let run = (timestamp, distribution, times) => {
  let rec run = (times, result) =>
    if (times > 0) {
      run(
        times - 1,
        [
          Distribution.MonthDistribution.happens(timestamp, distribution),
          ...result,
        ],
      );
    } else {
      result;
    };
  run(times, []);
};

describe("month distribution", ({test, testOnly}) => {
  test("without nothing", ({expect}) => {
    let timestamp = 0.;
    let m =
      Distribution.MDistribution.init(timestamp)
      |> Distribution.MDistribution.add(~data=Distribution.PerMonth(0))
      |> Distribution.MDistribution.build;
    let distribution =
      Distribution.MonthDistribution.ForEver(Distribution.Spread(m))
      |> Distribution.MonthDistribution.create;
    let result =
      Distribution.MonthDistribution.happens(timestamp, distribution);

    expect.bool(result).toBe(false);
  });
  test("without a huge amount", ({expect}) => {
    let timestamp = 0.;
    let m =
      Distribution.MDistribution.init(timestamp)
      |> Distribution.MDistribution.add(~data=Distribution.PerMonth(10000))
      |> Distribution.MDistribution.build;
    let distribution =
      Distribution.MonthDistribution.ForEver(Distribution.Spread(m))
      |> Distribution.MonthDistribution.create;
    let result =
      Distribution.MonthDistribution.happens(timestamp, distribution);

    expect.bool(result).toBe(true);
  });

  test("is the frequency correct one in", ({expect}) => {
    let timestamp = 0.;
    let distribution =
      Distribution.MonthDistribution.Number(1, Steady(OneIn(1)))
      |> Distribution.MonthDistribution.create;
    expect.bool(
      Distribution.MonthDistribution.happens(timestamp, distribution),
    ).
      toBe(
      true,
    );
  });

  test("is the frequency correct one in", ({expect}) => {
    let timestamp = 0.;
    let distribution =
      Distribution.MonthDistribution.Number(1, Steady(OneIn(1)))
      |> Distribution.MonthDistribution.create;

    let result = run(timestamp, distribution, 24 * 60 / 10);

    let amount = List.count(~f=i => i, result);
    expect.bool(amount == 1).toBe(true);
  });

  testOnly("is the frequency correct one in", ({expect}) => {
    let foo = timestamp => {
      let distribution =
        Distribution.MonthDistribution.Number(10, Steady(OneIn(1)))
        |> Distribution.MonthDistribution.create;

      let result = run(timestamp, distribution, 24 * 60 / 10);

      List.count(~f=i => i, result);
    };
    let timestamp = 0.;
    let amount = foo(timestamp);
    let amount2 = foo(timestamp);
    expect.bool(amount == 10).toBe(true);
    expect.bool(amount2 == 10).toBe(true);
  });

  test("is the frequency correct per day", ({expect}) => {
    let timestamp = 0.;
    let m =
      Distribution.MDistribution.init(timestamp)
      |> Distribution.MDistribution.add(~data=Distribution.PerDay(10))
      |> Distribution.MDistribution.build;
    let distribution =
      Distribution.MonthDistribution.ForEver(Distribution.Spread(m))
      |> Distribution.MonthDistribution.create;
    let result = run(timestamp, distribution, 24 * 60 / 10);

    let amount = List.count(~f=i => i, result);
    expect.bool(amount < 20).toBe(true);
  });
  test("is the frequency correct per month", ({expect}) => {
    let timestamp = 0.;
    let m =
      Distribution.MDistribution.init(timestamp)
      |> Distribution.MDistribution.add(~data=Distribution.PerMonth(10))
      |> Distribution.MDistribution.build;
    let distribution =
      Distribution.MonthDistribution.ForEver(Distribution.Spread(m))
      |> Distribution.MonthDistribution.create;
    let result = run(timestamp, distribution, 24 * 60 / 10 * 31);

    let amount = List.count(~f=i => i, result);
    expect.bool(amount < 20).toBe(true);
  });
});
