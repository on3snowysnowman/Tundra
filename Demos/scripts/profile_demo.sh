perf record -g ./bin/demo
perf report --call-graph > perf_report_results.txt
perf stat -r 10 -d ./bin/demo
