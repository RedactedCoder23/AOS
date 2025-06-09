#!/bin/sh
# Demonstrate branch persistence
rm -f "$HOME/.aos/branches.json"

cat <<EOF | ./build/host_test > /tmp/persist1.log
branch create alpha
branch create beta
branch save
exit
EOF

cat <<EOF | ./build/host_test > /tmp/persist2.log
branch load
branch tree
exit
EOF

cat /tmp/persist2.log
