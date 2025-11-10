import { Component, signal } from '@angular/core';
import { RouterOutlet } from '@angular/router';
import { Dashboard } from './dashboard/dashboard';
import { FoodSchedule } from './food-schedule/food-schedule';

@Component({
  selector: 'app-root',
  imports: [RouterOutlet, Dashboard, FoodSchedule],
  templateUrl: './app.html',
  styleUrl: './app.css',
})
export class App {
  protected readonly title = signal('FoodDPet');
}
