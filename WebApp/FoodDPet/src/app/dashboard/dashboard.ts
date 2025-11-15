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
  isRefilling: boolean = false;

  constructor(private dashboardService: DashboardService) {
    this.fullnessStatus = this.dashboardService.getFullnessStatus();
    this.fullnessPercentage = this.dashboardService.getFullnessPercentage();
    this.requestHistory = this.dashboardService.getRequestHistory();
  }

  refillDispenser() {
    this.isRefilling = true;
    this.dashboardService.refillDispenser().subscribe({
      next: () => {
        this.fullnessPercentage = this.dashboardService.getFullnessPercentage();
        this.fullnessStatus = this.dashboardService.getFullnessStatus();
        this.isRefilling = false;
        alert('¡Dispensador rellenado exitosamente!');
      },
      error: () => {
        this.isRefilling = false;
        alert('Error al rellenar el dispensador');
      },
    });
  }

  completeRequest(request: FoodRequest) {
    this.dashboardService.completeRequest(request);
    this.requestHistory = this.dashboardService.getRequestHistory();
  }

  cancelRequest(request: FoodRequest) {
    this.dashboardService.cancelRequest(request);
    this.requestHistory = this.dashboardService.getRequestHistory();
  }
}
