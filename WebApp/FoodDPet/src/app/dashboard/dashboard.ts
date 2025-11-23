import { Component, OnInit, OnDestroy } from '@angular/core';
import { CardModule } from 'primeng/card';
import { DashboardService, FoodRequest } from '../service/dashboard-service';
import { TableModule } from 'primeng/table';
import { CommonModule } from '@angular/common';
import { TagModule } from 'primeng/tag';
import { ButtonModule } from 'primeng/button';
import { Subscription } from 'rxjs';

@Component({
  selector: 'app-dashboard',
  imports: [CardModule, TableModule, CommonModule, TagModule, ButtonModule],
  templateUrl: './dashboard.html',
})
export class Dashboard implements OnInit, OnDestroy {
  fullnessStatus: string = 'Cargando...';
  fullnessPercentage: number = 0;
  requestHistory: FoodRequest[] = [];
  isRefilling: boolean = false;

  private subscriptions: Subscription[] = [];

  constructor(private dashboardService: DashboardService) {}

  ngOnInit() {
    // Suscribirse a los observables para recibir actualizaciones
    this.subscriptions.push(
      this.dashboardService.fullnessStatus$.subscribe((status) => {
        this.fullnessStatus = status;
      })
    );

    this.subscriptions.push(
      this.dashboardService.percentFullness$.subscribe((percentage) => {
        this.fullnessPercentage = percentage;
      })
    );

    this.subscriptions.push(
      this.dashboardService.requestHistory$.subscribe((history) => {
        this.requestHistory = history;
      })
    );
  }

  ngOnDestroy() {
    // Limpiar suscripciones
    this.subscriptions.forEach((sub) => sub.unsubscribe());
  }

  refillDispenser() {
    this.isRefilling = true;
    this.dashboardService.refillDispenser().subscribe({
      next: () => {
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
  }

  cancelRequest(request: FoodRequest) {
    this.dashboardService.cancelRequest(request);
  }
}
