# SEQUENTIAL
echo "Sequential Program:"
gcc -o seq_main seq_main.c -lm && ./seq_main papagan.jpg output_seq.jpg && rm seq_main

# OMP
echo -e "\nOMP Program:"
gcc -fopenmp -o omp_main omp_main.c -lm && ./omp_main papagan.jpg output_seq.jpg && rm omp_main

# COMPARE OMP & SEQUENTIAL OUTPUTS
echo -e "\nComparing Seq & OMP Outputs:"
gcc -o compare_outputs compare_outputs.c -lm && ./compare_outputs output.jpg output_seq.jpg && rm compare_outputs