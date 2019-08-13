type frequency =
  | PerDay(int)
  | PerMonth(int)
  | OneIn(int);

module Month: {
  type t;

  let fromTimestamp: float => t;
  let addMonth: t => t;
};

module MDistribution: {
  type t;
  type partial;

  let init: float => partial;
  let add: (partial, ~data: frequency) => partial;
  let build: partial => t;
};

module MonthDistribution: {
  type t =
    | Number(int, distribution)
    | Never
    | ForEver(distribution)
  and distribution =
    | Steady(frequency)
    | Spread(MDistribution.t);

  let happens: (float, t) => (bool, t);
};

module PercentageDistribution: {
  type t('a);
  type partial('a);

  let empty: unit => partial('a);
  let add: (partial('a), ~i: int, ~outcome: 'a) => partial('a);
  let rest: (partial('a), ~outcome: 'a) => t('a);
  let pick: t('a) => 'a;
};

let gaussianCapped:
  (~mu: float, ~sigma: float, ~lowerBound: float, ~upperBound: float) => float;

let happens: frequency => bool;
let randomFromList: list('a) => option('a);
