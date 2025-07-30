perf record -g ./bin/demo
perf report --call-graph > perf_report_results.txt

perf stat -d -r 10 ./bin/demo

