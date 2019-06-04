type frequency =
  | PerDay(int)
  | OneIn(int);

let minutesPerDay = 24 * 60;

let happens = frequency => {
  switch (frequency) {
  | PerDay(times) => Random.int(minutesPerDay / times) == 0
  | OneIn(x) => Random.int(x) == 0
  };
};

module D = {
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

let listRandom = l => {
  let length = List.length(l);
  let i = Random.int(length);
  List.nth(l, i);
};
