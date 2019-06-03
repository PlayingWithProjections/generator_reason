[@deriving show]
type event = {
  id: Uuid.t,
  timestamp: int,
  type_: payload,
}
and payload =
  | PlayerHasRegistered{
      playerId: Uuid.t,
      lastName: string,
      firstName: string,
    }
  | QuizWasCreated{
      quizId: Uuid.t,
      ownerId: Uuid.t,
      quizTitle: string,
    }
  | QuestionAddedToQuiz{
      quizId: Uuid.t,
      questionId: Uuid.t,
      question: string,
      answer: string,
    }
  | QuizWasPublished{quizId: Uuid.t}
  | GameWasOpened{
      quizId: Uuid.t,
      gameId: Uuid.t,
    }
  | GameWasCancelled{
      gameId: Uuid.t,
    }
  | GameWasStarted{
      gameId: Uuid.t,
    }
  | PlayerJoinedGame{
      playerId: Uuid.t,
      gameId: Uuid.t,
    }
  | QuestionWasAsked{
      gameId: Uuid.t,
      questionId: Uuid.t,
    }
  | TimeHasExperid{
      questionId: Uuid.t,
      playerId: Uuid.t,
      gameId: Uuid.t,
    }
  | GameWasFinished{gameId: Uuid.t}
  | AnswerWasGiven{
      questionId: Uuid.t,
      playerId: Uuid.t,
      gameId: Uuid.t,
      answer: string,
    }
  | QuestionWasCompleted{
      questionId: Uuid.t,
      gameId: Uuid.t,
    };

let create = (~timestamp, ~type_) => {id: Uuid.generateId(), type_, timestamp};
