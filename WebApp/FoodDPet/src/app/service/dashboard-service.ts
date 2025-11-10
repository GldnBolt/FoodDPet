import { Injectable } from '@angular/core';

export interface FoodRequest {
  hora: Date;
  estado: string;
}

@Injectable({
  providedIn: 'root',
})
export class DashboardService {
  // Example data for pet food fullness status
  // Replace with connection to microcontroller later
  percentFullness: number = 75;
  requestHistory: FoodRequest[] = [
    { hora: new Date('2024-10-01T10:00:00'), estado: 'Pending' },
    { hora: new Date('2024-10-02T14:30:00'), estado: 'Cancelled' },
    { hora: new Date('2024-10-03T09:15:00'), estado: 'Completed' },
    { hora: new Date('2024-10-04T16:45:00'), estado: 'Pending' },
  ];

  getFullnessStatus(): string {
    if (this.percentFullness >= 75) {
      return 'Alto';
    } else if (this.percentFullness >= 50) {
      return 'Medio';
    } else {
      return 'Bajo';
    }
  }

  getFullnessPercentage(): number {
    return this.percentFullness;
  }

  getRequestHistory(): FoodRequest[] {
    return this.requestHistory;
  }
}
