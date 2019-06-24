open! Base;

type frequency =
  | PerDay(int)
  | PerMonth(int)
  | OneIn(int);

let minutesPerDay = 24 * 60;
let minutesPerMonth = minutesPerDay * 30;

let happens = frequency => {
  switch (frequency) {
  | PerDay(times) => Random.int(minutesPerDay / times) == 0
  | PerMonth(times) => Random.int(minutesPerMonth / times) == 0
  | OneIn(x) => Random.int(x) == 0
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
    let date =
      CalendarLib.Calendar.Precise.from_unixfloat(Float.of_int(timestamp));
    let year = CalendarLib.Calendar.Precise.year(date);
    let month = CalendarLib.Calendar.Precise.year(date);
    {year, month};
  };
};

module MonthDistribution = {
  type spread =
    Map.t(Month.t, frequency, Month.comparator_witness);
  type month = {
    year: int,
    month: int,
  };
  type howMany =
    | Number(int)
    | Never
    | ForEver;
  type distribution =
    | Steady(frequency)
    | Spread(spread);
  type t = (howMany, distribution);
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
    switch (t) {
    | (Never, _) => (false, t)
    | (ForEver, distribution) => (trueOrFalse(distribution), t)
    | (Number(0), _) => (false, t)
    | (Number(x), distribution) =>
      if (trueOrFalse(distribution)) {
        (true, (Number(x - 1), distribution));
      } else {
        (false, (Number(x), distribution));
      }
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
