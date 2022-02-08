let month_int date =
  let month = CalendarLib.Calendar.Precise.month date in
  match month with
  | CalendarLib.Calendar.Jan -> 1
  | CalendarLib.Calendar.Feb -> 2
  | CalendarLib.Calendar.Mar -> 3
  | CalendarLib.Calendar.Apr -> 4
  | CalendarLib.Calendar.May -> 5
  | CalendarLib.Calendar.Jun -> 6
  | CalendarLib.Calendar.Jul -> 7
  | CalendarLib.Calendar.Aug -> 8
  | CalendarLib.Calendar.Sep -> 9
  | CalendarLib.Calendar.Oct -> 10
  | CalendarLib.Calendar.Nov -> 11
  | CalendarLib.Calendar.Dec -> 12

type frequency = PerDay of int | PerMonth of int | OneIn of int

let minutesPerDay = 24 * 60 / 10
let minutesPerMonth = minutesPerDay * 30

let happens frequency =
  match frequency with
  | ((OneIn 0) [@explicit_arity]) -> false
  | ((OneIn x) [@explicit_arity]) -> Random.int x = 0
  | ((PerDay 0) [@explicit_arity]) -> false
  | ((PerDay times) [@explicit_arity]) ->
      let x = minutesPerDay / times in
      if x = 0 then true else Random.int x = 0
  | ((PerMonth 0) [@explicit_arity]) -> false
  | ((PerMonth times) [@explicit_arity]) ->
      let x = minutesPerMonth / times in
      if x = 0 then true else Random.int x = 0

module Month = struct
  module T = struct
    type t = { year : int; month : int }

    let compare t1 t2 =
      let cmpYear = compare t1.year t2.year in
      if cmpYear <> 0 then cmpYear else compare t1.month t2.month

    let sexp_of_t t : Sexp.t =
      (Sexp.List
         [
           (Sexp.Atom (Int.to_string t.year) [@explicit_arity]);
           (Sexp.Atom (Int.to_string t.month) [@explicit_arity]);
         ]
      [@explicit_arity])
  end

  include T
  include Comparator.Make (T)

  let fromTimestamp timestamp =
    let date = CalendarLib.Calendar.Precise.from_unixfloat timestamp in
    let year = CalendarLib.Calendar.Precise.year date in
    { year; month = month_int date }

  let addMonth { year; month } =
    if month = 12 then { year = year + 1; month = 1 }
    else { year; month = month + 1 }
end

module MDistribution = struct
  type t = (Month.t, frequency, Month.comparator_witness) Map.t
  type partial = t * Month.t

  let init timestamp =
    let key = Month.fromTimestamp timestamp in
    (Map.empty (module Month), key)

  let add (t, key) ~data = (Map.set t ~key ~data, Month.addMonth key)
  let build (t, _) = t
end

type distribution = Steady of frequency | Spread of MDistribution.t

module MonthDistribution = struct
  type t = { distribution : distribution; mutable number : int }
  type u = Number of int * distribution | Never | ForEver of distribution

  let create u =
    match u with
    | Never ->
        {
          distribution = Steady (PerMonth 0 [@explicit_arity]) [@explicit_arity];
          number = 0;
        }
    | ((Number (number, distribution)) [@explicit_arity]) ->
        { distribution; number }
    | ((ForEver distribution) [@explicit_arity]) ->
        { distribution; number = 1000000000 }

  let happens timestamp t =
    let trueOrFalse distribution =
      match distribution with
      | ((Steady perPeriod) [@explicit_arity]) -> happens perPeriod
      | ((Spread spread) [@explicit_arity]) -> (
          let month = Month.fromTimestamp timestamp in
          match Map.find spread month with
          | None -> false
          | ((Some frequency) [@explicit_arity]) -> happens frequency)
    in
    if t.number > 0 && trueOrFalse t.distribution then (
      t.number <- t.number - 1;
      true)
    else false
end

module PercentageDistribution = struct
  open! Base [@@ocaml.doc
               "\n\
                \t * A module for easily creating distributions that you want \
                to pick.\n\
                \t * An example:\n\
                \t *     empty()\n\
                \t *     |> add(~i=5, ~outcome=\"foo\")\n\
                \t *     |> add(~i=10, ~outcome=\"bar\")\n\
                \t *     |> rest(~outcome=\"baz\")\n\
                \t *     |> pick\n\
                \t *\n\
                \t * Will pick \"foo\" in 5% of the cases, \"bar\" in 10% and \
                the rest will be \"baz\"\n\
                \t "]

  type 'a t = (float * (unit -> 'a)) list

  type 'a partial = {
    distributions : (float * (unit -> 'a)) list;
    currentPosition : float;
  }

  let empty () = { currentPosition = 0.; distributions = [] }

  let add partial ~i ~outcome =
    let currentPosition = partial.currentPosition +. i in
    {
      distributions =
        (partial.currentPosition, outcome) :: partial.distributions;
      currentPosition;
    }

  let rest partial ~outcome =
    (partial.currentPosition, outcome) :: partial.distributions

  let pick distribution =
    let percentage = Random.float 100. in
    List.find_map_exn
      ~f:(fun (d, outcome) ->
        match Float.compare percentage d >= 0 with
        | true -> Some (outcome ()) [@explicit_arity]
        | false -> None)
      distribution
end

let randomFromList l =
  match l with
  | [] -> None
  | _ ->
      let length = List.length l in
      let i = Random.int length in
      (Some (List.nth_exn l i) [@explicit_arity])

let gaussianCapped ~mu ~sigma ~lowerBound ~upperBound =
  let open! Poly in
  let r =
    mu
    +. sigma
       *. Float.sqrt (-2. *. Float.log (Random.float 1.))
       *. Float.cos (2. *. Float.pi *. Random.float 1.)
  in
  match (r < lowerBound, r > upperBound) with
  | true, _ -> lowerBound
  | _, true -> upperBound
  | _ -> r
