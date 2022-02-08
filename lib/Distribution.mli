type frequency = PerDay of int | PerMonth of int | OneIn of int

module Month : sig
  type t

  val fromTimestamp : float -> t
  val addMonth : t -> t
end

module MDistribution : sig
  type t
  type partial

  val init : float -> partial
  val add : partial -> data:frequency -> partial
  val build : partial -> t
end

type distribution = Steady of frequency | Spread of MDistribution.t

module MonthDistribution : sig
  type t
  type u = Number of int * distribution | Never | ForEver of distribution

  val create : u -> t
  val happens : float -> t -> bool
end

module PercentageDistribution : sig
  type 'a t
  type 'a partial

  val empty : unit -> 'a partial
  val add : 'a partial -> i:float -> outcome:(unit -> 'a) -> 'a partial
  val rest : 'a partial -> outcome:(unit -> 'a) -> 'a t
  val pick : 'a t -> 'a
end

val gaussianCapped :
  mu:float -> sigma:float -> lowerBound:float -> upperBound:float -> float

val happens : frequency -> bool
val randomFromList : 'a list -> 'a option
