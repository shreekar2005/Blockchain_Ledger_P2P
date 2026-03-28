import numpy as np
import matplotlib.pyplot as plt

def run_analysis():
    mean_block_time = 2.0
    node_hash_power_percent = 20.0
    num_cycles = 100

    arrival_rate = (node_hash_power_percent * (1.0 / mean_block_time)) / 100.0
    
    waiting_times = np.random.exponential(scale=(1.0 / arrival_rate), size=num_cycles)

    plt.figure(figsize=(12, 5))

    plt.subplot(1, 2, 1)
    plt.hist(waiting_times, bins=15, density=True, alpha=0.7, color='skyblue', edgecolor='black')
    plt.title(f"Waiting Time ($T_k$) Distribution\n(Hash Power={node_hash_power_percent}%)")
    plt.xlabel("Waiting Time $T_k$ (seconds)")
    plt.ylabel("Probability Density")
    plt.grid(axis='y', linestyle='--', alpha=0.7)

    hash_power_range = np.arange(1, 101, 1)
    lambda_values = (hash_power_range * (1.0 / mean_block_time)) / 100.0

    plt.subplot(1, 2, 2)
    plt.plot(hash_power_range, lambda_values, color='red', linewidth=2)
    plt.title("Arrival Rate ($\lambda$) vs. Hash Power")
    plt.xlabel("Hash Power Percentage (%)")
    plt.ylabel("Lambda ($\lambda$) Parameter")
    plt.grid(linestyle='--', alpha=0.7)

    plt.tight_layout()
    plt.show()

if __name__ == "__main__":
    run_analysis()
