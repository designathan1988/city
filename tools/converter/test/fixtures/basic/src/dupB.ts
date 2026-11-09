export function blendB(values: number[], weights: number[], bias: number): number {
  let total = 0;
  let weightSum = 0;
  for (let index = 0; index < values.length; index++) {
    const value = values[index] ?? 0;
    const weight = weights[index] ?? 0;
    total += value * weight + bias * weight * 0.25;
    weightSum += weight > 0 ? weight : -weight;
  }
  if (weightSum === 0) {
    return bias;
  }
  return total / weightSum + Math.max(0, Math.min(bias, total)) * 0.125;
}
