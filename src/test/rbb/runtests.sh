#!/usr/bin/env bash

echo "Running script tests"

for t in *tests.rbb; do
    ../inst/bin/rbbs $t -p ../../mod -p ../inst/lib/rbb -m base
done

