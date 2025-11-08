import { Component } from '@angular/core';
import { CardModule } from 'primeng/card';
import { DashboardService, FoodRequest } from '../service/dashboard-service';
import { TableModule } from 'primeng/table';
import { CommonModule } from '@angular/common';
import { TagModule } from 'primeng/tag';
import { ButtonModule } from 'primeng/button';

@Component({
  selector: 'app-dashboard',
  imports: [CardModule, TableModule, CommonModule, TagModule, ButtonModule],
  templateUrl: './dashboard.html',
})
export class Dashboard {
  fullnessStatus: string;
  fullnessPercentage: number;
  requestHistory: FoodRequest[];

  constructor(private dashboardService: DashboardService) {
    this.fullnessStatus = this.dashboardService.getFullnessStatus();
    this.fullnessPercentage = this.dashboardService.getFullnessPercentage();
    this.requestHistory = this.dashboardService.getRequestHistory();
  }

  completeRequest(request: FoodRequest) {
    // Implement completion logic here
  }

  cancelRequest(request: FoodRequest) {
    // Implement cancellation logic here
  }
}
