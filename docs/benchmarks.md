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