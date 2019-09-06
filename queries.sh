#!/usr/bin/env bash
echo "Total"; cat $1 | jq 'length'
echo "GameWasStarted"; cat $1 | jq '[.[] | select(.type == "GameWasStarted")] | length'
echo "GameWasFinished"; cat $1 | jq '[.[] | select(.type == "GameWasFinished")] | length'
echo "GameWasCancelled"; cat $1 | jq '[.[] | select(.type == "GameWasCancelled")] | length'
echo "PlayerHasRegisterd"; cat $1 | jq '[.[] | select(.type == "PlayerHasRegistered") | .payload.owner_id] | length'
echo "QuizWasCreated"; cat $1 | jq '[.[] | select(.type == "QuizWasCreated") | .payload.owner_id] | sort | length'
echo "GameWasOpened"; cat $1 | jq '[.[] | select(.type == "GameWasOpened")] | length'
echo "PlayerJoinedGame"; cat $1 | jq '[.[] | select(.type == "PlayerJoinedGame")] | length'
