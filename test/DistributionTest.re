open TestFramework;
open Lib;

describe("month distribution", ({test}) => {
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
});
