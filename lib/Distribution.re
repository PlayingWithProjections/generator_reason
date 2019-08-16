open! Base;

let month_int = date => {
  let month = CalendarLib.Calendar.Precise.month(date);
  switch (month) {
  | CalendarLib.Calendar.Jan => 1
  | CalendarLib.Calendar.Feb => 2
  | CalendarLib.Calendar.Mar => 3
  | CalendarLib.Calendar.Apr => 4
  | CalendarLib.Calendar.May => 5
  | CalendarLib.Calendar.Jun => 6
  | CalendarLib.Calendar.Jul => 7
  | CalendarLib.Calendar.Aug => 8
  | CalendarLib.Calendar.Sep => 9
  | CalendarLib.Calendar.Oct => 10
  | CalendarLib.Calendar.Nov => 11
  | CalendarLib.Calendar.Dec => 12
  };
};

type frequency =
  | PerDay(int)
  | PerMonth(int)
  | OneIn(int);

let minutesPerDay = 24 * 60 / 10;
let minutesPerMonth = minutesPerDay * 30;

let happens = frequency => {
  switch (frequency) {
  | OneIn(0) => false
  | OneIn(x) => Random.int(x) == 0
  | PerDay(0) => false
  | PerDay(times) =>
    let x = minutesPerDay / times;
    if (x == 0) {
      true;
    } else {
      Random.int(x) == 0;
    };
  | PerMonth(0) => false
  | PerMonth(times) =>
    let x = minutesPerMonth / times;
    if (x == 0) {
      true;
    } else {
      Random.int(x) == 0;
    };
  };
};

module Month = {
  module T = {
    type t = {
      year: int,
      month: int,
    };
    let compare = (t1, t2) => {
      let cmpYear = compare(t1.year, t2.year);
      if (cmpYear != 0) {
        cmpYear;
      } else {
        compare(t1.month, t2.month);
      };
    };

    let sexp_of_t = (t): Sexp.t =>
      Sexp.List([
        Sexp.Atom(Int.to_string(t.year)),
        Sexp.Atom(Int.to_string(t.month)),
      ]);
  };

  include T;
  include Comparator.Make(T);

  let fromTimestamp = timestamp => {
    let date = CalendarLib.Calendar.Precise.from_unixfloat(timestamp);
    let year = CalendarLib.Calendar.Precise.year(date);
    {year, month: month_int(date)};
  };

  let addMonth = ({year, month}) =>
    if (month == 12) {
      {year: year + 1, month: 1};
    } else {
      {year, month: month + 1};
    };
};

module MDistribution = {
  type t = Map.t(Month.t, frequency, Month.comparator_witness);
  type partial = (t, Month.t);

  let init = timestamp => {
    let key = Month.fromTimestamp(timestamp);
    (Map.empty((module Month)), key);
  };

  let add = ((t, key), ~data) => {
    (Map.set(t, ~key, ~data), Month.addMonth(key));
  };

  let build = ((t, _)) => t;
};

type distribution =
  | Steady(frequency)
  | Spread(MDistribution.t);

module MonthDistribution = {
  type t = {
    distribution,
    mutable number: int,
  };

  type u =
    | Number(int, distribution)
    | Never
    | ForEver(distribution);

  let create = u => {
    switch (u) {
    | Never => {distribution: Steady(PerMonth(0)), number: 0}
    | Number(number, distribution) => {distribution, number}
    | ForEver(distribution) => {distribution, number: 1000000000}
    };
  };

  let happens = (timestamp, t) => {
    let trueOrFalse = distribution => {
      switch (distribution) {
      | Steady(perPeriod) => happens(perPeriod)
      | Spread(spread) =>
        let month = Month.fromTimestamp(timestamp);
        switch (Map.find(spread, month)) {
        | None => false
        | Some(frequency) => happens(frequency)
        };
      };
    };
    if (t.number > 0 && trueOrFalse(t.distribution)) {
      t.number = t.number - 1;
      true;
    } else {
      false;
    };
  };
};

module PercentageDistribution = {
  /**
	 * A module for easily creating distributions that you want to pick.
	 * An example:
	 *     empty()
	 *     |> add(~i=5, ~outcome="foo")
	 *     |> add(~i=10, ~outcome="bar")
	 *     |> rest(~outcome="baz")
	 *     |> pick
	 *
	 * Will pick "foo" in 5% of the cases, "bar" in 10% and the rest will be "baz"
	 */
  open! Base;
  type t('a) = list((int, 'a));
  type partial('a) = {
    distributions: list((int, 'a)),
    currentPosition: int,
  };

  let empty = () => {currentPosition: 0, distributions: []};

  let add = (partial, ~i, ~outcome) => {
    let currentPosition = partial.currentPosition + i;
    {
      distributions: [
        (partial.currentPosition, outcome),
        ...partial.distributions,
      ],
      currentPosition,
    };
  };

  let rest = (partial, ~outcome) => [
    (partial.currentPosition, outcome),
    ...partial.distributions,
  ];

  let pick = distribution => {
    let percentage = Random.int(100);
    List.find_map_exn(
      ~f=((d, outcome)) => percentage >= d ? Some(outcome) : None,
      distribution,
    );
  };
};

let randomFromList = l => {
  switch (l) {
  | [] => None
  | _ =>
    let length = List.length(l);
    let i = Random.int(length);
    Some(List.nth_exn(l, i));
  };
};

let gaussianCapped = (~mu, ~sigma, ~lowerBound, ~upperBound) => {
  open! Poly;
  /* https://en.wikipedia.org/wiki/Box%E2%80%93Muller_transform */
  let r =
    mu
    +. sigma
    *. Float.sqrt((-2.) *. Float.log(Random.float(1.)))
    *. Float.cos(2. *. Float.pi *. Random.float(1.));
  switch (r < lowerBound, r > upperBound) {
  | (true, _) => lowerBound
  | (_, true) => upperBound
  | _ => r
  };
};
