# Benchmarks
Current benchmark numbers on my Lenovo Yoga 7 Intel Core Ultra 7 258V with Win11

|               ns/op |                op/s |    err% |     total | benchmark
|--------------------:|--------------------:|--------:|----------:|:----------
|            2,114.73 |          472,873.40 |    3.0% |      0.52 | `PackageReference Parse Name Only`
|            3,377.58 |          296,069.79 |    1.2% |      0.81 | `PackageReference Parse Language, User, Name and Version`
|               67.65 |       14,782,358.44 |    1.1% |      0.02 | `ValueTableReader Deserialize Empty`
|            1,518.21 |          658,671.50 |    1.3% |      0.36 | `ValueTableReader Deserialize Complex`
|              113.23 |        8,831,847.65 |    2.8% |      0.03 | `OperationResultsReader Deserialize Empty`
|              702.65 |        1,423,178.43 |    1.8% |      0.17 | `OperationResultsReader Deserialize Complex`
|            1,071.80 |          933,012.90 |    1.0% |      0.26 | `RecipeSML Deserialize Simple`
|            6,152.02 |          162,548.28 |    1.0% |      1.48 | `RecipeSML Deserialize Complex`

Current benchmark numbers on my Lenovo Yoga 7 Intel Core Ultra 7 258V with Ubuntu 24.04 LTS

|               ns/op |                op/s |    err% |     total | benchmark
|--------------------:|--------------------:|--------:|----------:|:----------
|            1,965.90 |          508,674.16 |    0.6% |      0.49 | `PackageReference Parse Name Only`
|            2,733.01 |          365,896.80 |    1.5% |      0.66 | `PackageReference Parse Language, User, Name and Version`
|               99.55 |       10,045,681.30 |    0.4% |      0.02 | `ValueTableReader Deserialize Empty`
|            4,901.49 |          204,019.48 |    0.7% |      1.17 | `ValueTableReader Deserialize Complex`
|              139.18 |        7,185,191.42 |    0.7% |      0.03 | `OperationResultsReader Deserialize Empty`
|            3,551.31 |          281,586.57 |    0.7% |      0.85 | `OperationResultsReader Deserialize Complex`
|            5,955.45 |          167,913.35 |    0.6% |      1.42 | `RecipeSML Deserialize Simple`
|           19,341.45 |           51,702.42 |    1.1% |      4.64 | `RecipeSML Deserialize Complex`