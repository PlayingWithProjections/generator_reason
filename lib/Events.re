[@deriving show]
type event =
  | PlayerHasRegistered{
      playerId: Uuidm.t,
      lastName: string,
      firstName: string,
      timeStamp: int,
    }
  | QuizWasCreated{
      id: Uuidm.t,
      ownerId: Uuidm.t,
      timeStamp: int,
    }
  | GameWasOpened{
      quizId: Uuidm.t,
      gameId: Uuidm.t,
    }
  | PlayerJoinedGame{
      playerId: Uuidm.t,
      gameId: Uuidm.t,
    }
  | MinuteHasPassed{timeStamp: int};
