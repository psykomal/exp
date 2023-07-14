#!/bin/bash

for i in {1..10}; do
  nc 127.0.0.1 8080 &
done

# Wait for all background processes to finish
wait
