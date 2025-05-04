#!/bin/bash
for txt_file in tests/*.txt; do
    echo "=== Test name: $txt_file ==="
    ./app.out "$txt_file"
    echo "=== End of test ==="
done