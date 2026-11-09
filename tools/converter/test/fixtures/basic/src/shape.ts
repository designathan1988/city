export interface Shape {
  area(): number;
  onChange?: (area: number) => void;
}

export class Square implements Shape {
  side = 2;
  area(): number {
    return this.side * this.side;
  }
}

export class Circle implements Shape {
  radius = 1;
  area(): number {
    return Math.PI * this.radius * this.radius;
  }
}
