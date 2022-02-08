type t

val generateId : unit -> t
val to_string : ?upper:bool -> t -> string
val pp : Formatter.t -> t -> unit
